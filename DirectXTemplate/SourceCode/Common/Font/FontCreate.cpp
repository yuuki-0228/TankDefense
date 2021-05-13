#include "FontCreate.h"

CFontCreate::CFontCreate()
	: m_pDevice11	( nullptr )
	, m_pContext11	( nullptr )
{
	FontAvailable();	// フォントを利用可能にする.
}

CFontCreate::CFontCreate( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11 )
	: m_pDevice11	( pDevice11 )
	, m_pContext11	( pContext11 )
{
	FontAvailable();	// フォントを利用可能にする.
}

CFontCreate::~CFontCreate()
{
	// 利用可能にしたフォントを破棄する.
	DESIGNVECTOR design;
	RemoveFontResourceEx( TEXT(FONT_PATH), FR_PRIVATE, &design );
}

//-----------------------------------.
// フォント画像の作成.
//-----------------------------------.
HRESULT CFontCreate::CreateFontTexture2D( const char* c, ID3D11ShaderResourceView** resource )
{
	if( m_pDevice11 == nullptr ) return E_FAIL;
	if( m_pContext11 == nullptr ) return E_FAIL;

	// 文字コード取得
	UINT code = 0;
#if _UNICODE
	// unicodeの場合、文字コードは単純にワイド文字のUINT変換です
	code = (UINT)*c;
#else
	// マルチバイト文字の場合、
	// 1バイト文字のコードは1バイト目のUINT変換、
	// 2バイト文字のコードは[先導コード]*256 + [文字コード]です
	if( IsDBCSLeadByte(*c) ){
		code = (BYTE)c[0]<<8 | (BYTE)c[1];
	} else {
		code = c[0];
	}
#endif

	// フォントの生成
	LOGFONT lf = { 
		FONT_BMP_SIZE,					// 文字セルまたは文字の高さ.
		0,								// 平均文字幅.
		0,								// 文字送りの方向とX軸との角度.
		0,								// ベースラインとX軸との角度.
		0,								// フォントの太さ.
		0,								// イタリック体の指定.
		0,								// 下線付き指定.
		0,								// 打消し線付き指定.
		SHIFTJIS_CHARSET,				// キャラクタセット.
		OUT_TT_ONLY_PRECIS,				// 出力精度.
		CLIP_DEFAULT_PRECIS,			// クリッピングの精度.
		PROOF_QUALITY,					// 出力品質.
		FIXED_PITCH | FF_MODERN,		// ピッチとファミリ.
		TEXT("Ricty Diminished")		// フォント名.
	};

	HFONT hFont = nullptr;
	hFont = CreateFontIndirect( &lf );
	if(  hFont == nullptr ) return E_FAIL;

	HDC hdc			= nullptr;
	HFONT oldFont	= nullptr;
	// デバイスコンテキスト取得.
	// デバイスにフォントを持たせないとGetGlyphOutline関数はエラーとなる.
	hdc = GetDC( nullptr );
	oldFont = (HFONT)SelectObject( hdc, hFont );

	// フォントビットマップ取得.
	TEXTMETRIC TM;
	GetTextMetrics( hdc, &TM );
	GLYPHMETRICS GM;
	CONST MAT2 Mat = { {0,1}, {0,0}, {0,0}, {0,1} };
	DWORD size = GetGlyphOutline( hdc, code, GGO_GRAY4_BITMAP, &GM, 0, nullptr, &Mat );
	BYTE* ptr = new BYTE[size];
	GetGlyphOutline( hdc, code, GGO_GRAY4_BITMAP, &GM, size, ptr, &Mat );

	// デバイスコンテキストとフォントハンドルの開放
	SelectObject( hdc, oldFont );
	DeleteObject( hFont );
	ReleaseDC( nullptr, hdc );


	//--------------------------------
	// 書き込み可能テクスチャ作成
	// CPUで書き込みができるテクスチャを作成
	// テクスチャ作成
	D3D11_TEXTURE2D_DESC desc = CreateDesc( GM.gmCellIncX, TM.tmHeight );
	ID3D11Texture2D* texture2D = nullptr;

	if( FAILED( m_pDevice11->CreateTexture2D( &desc, 0, &texture2D ))){
		return E_FAIL;
	}

	D3D11_MAPPED_SUBRESOURCE hMappedResource;
	if( FAILED( m_pContext11->Map( 
		texture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &hMappedResource ))){
		return E_FAIL;
	}

	// データを取得する.
	BYTE* pBits = (BYTE*)hMappedResource.pData;
	
	// フォント情報の書き込み.
	// iOfs_x, iOfs_y : 書き出し位置(左上).
	// iBmp_w, iBmp_h : フォントビットマップの幅高.
	// Level : α値の段階 (GGO_GRAY4_BITMAPなので17段階).
	int iOfs_x = GM.gmptGlyphOrigin.x;
	int iOfs_y = TM.tmAscent - GM.gmptGlyphOrigin.y;
	int iBmp_w = GM.gmBlackBoxX + ( 4 - ( GM.gmBlackBoxX % 4 ) ) % 4 ;
	int iBmp_h = GM.gmBlackBoxY;
	int Level = 17;
	int x, y;
	DWORD Alpha, Color;
	memset( pBits, 0, hMappedResource.RowPitch * TM.tmHeight );
	for( y = iOfs_y; y < iOfs_y + iBmp_h; y++ ){
		for( x = iOfs_x; x < iOfs_x + iBmp_w; x++){
			Alpha = ( 255 * ptr[x - iOfs_x + iBmp_w * ( y - iOfs_y )] ) / ( Level - 1 );
			Color = 0x00ffffff | ( Alpha << 24 );

			memcpy( (BYTE*)pBits + hMappedResource.RowPitch * y + 4 * x, &Color, sizeof(DWORD) );
		}
	}
	m_pContext11->Unmap( texture2D, 0 );

	delete[] ptr;

	//-------------------------------------------------.
	// テクスチャ情報をシェーダーリソースビューにする.

	// テクスチャ情報を取得する.
	D3D11_TEXTURE2D_DESC texDesc;
	texture2D->GetDesc( &texDesc );

	// ShaderResourceViewの情報を作成する.
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory( &srvDesc, sizeof(srvDesc) );
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;

	if( FAILED( m_pDevice11->CreateShaderResourceView( texture2D, &srvDesc, resource ))){
		return E_FAIL;
	}

	return S_OK;
}

//-----------------------------------.
// フォントを利用可能にする.
//-----------------------------------.
int CFontCreate::FontAvailable()
{
	DESIGNVECTOR design;

	return AddFontResourceEx( 
		TEXT(FONT_PATH),	// フォントリソース名.
		FR_PRIVATE,			// プロセス終了時にインストールしたフォントを削除.
		&design );			// フォント構造体.
}

//-----------------------------------.
// Textur2D_Descを作成.
//-----------------------------------.
D3D11_TEXTURE2D_DESC CFontCreate::CreateDesc( UINT width, UINT height )
{
	D3D11_TEXTURE2D_DESC desc;
	memset( &desc, 0, sizeof( desc ) );
	desc.Width				= width;
	desc.Height				= height;
	desc.MipLevels			= 1;
	desc.ArraySize			= 1;
	desc.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;	// RGBA(255,255,255,255)タイプ
	desc.SampleDesc.Count	= 1;
	desc.Usage				= D3D11_USAGE_DYNAMIC;			// 動的（書き込みするための必須条件）
	desc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;	// シェーダリソースとして使う
	desc.CPUAccessFlags		= D3D11_CPU_ACCESS_WRITE;		// CPUからアクセスして書き込みOK

	return desc;
}