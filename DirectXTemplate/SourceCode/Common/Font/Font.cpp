#include "Font.h"
#include "..\..\Resource\FontResource\FontResource.h"
#include "..\Sprite\SpriteRender.h"

CFont::CFont()
	: m_pSpriteRender	( nullptr )
	, m_pVertexBuffer3D	( nullptr )
	, m_pVertexBufferUI	( nullptr )
{
	m_pSpriteRender = std::make_unique<CSpriteRender>();
}

CFont::~CFont()
{
	Release();
}

//---------------------------------.
// 初期化.
//---------------------------------.
HRESULT CFont::Init( ID3D11DeviceContext* pContext11 )
{
	if( FAILED( SetDevice( pContext11 ) ))				return E_FAIL;
	if( FAILED( m_pSpriteRender->Init( pContext11 ) ))	return E_FAIL;
	if( FAILED( CreateVerTexBuffer() ))					return E_FAIL;

	return S_OK;
}

//---------------------------------.
// 解放.
//---------------------------------.
void CFont::Release()
{
	SAFE_RELEASE( m_pVertexBufferUI );
	SAFE_RELEASE( m_pVertexBuffer3D );
}

//---------------------------------.
// UIで描画.
//---------------------------------.
void CFont::RenderUI( const std::string& text )
{
	// 文字数分ループ.
	float posX = m_Tranceform.Position.x;
	for( int i = 0; i < static_cast<int>(text.length()); i++ ){
		std::string f = text.substr( i, 1 );
		if( IsDBCSLeadByte( text[i] ) == TRUE ){
			f = text.substr( i++, 2 );	// 全角文字.
		} else {
			f = text.substr( i, 1 );	// 半角文字.
		}
		RenderFontUI( f.c_str() );
		m_Tranceform.Position.x += (FONT_SIZE) * m_Tranceform.Scale.x;
	}
	m_Tranceform.Position.x = posX;
}

//---------------------------------.
// 3Dで描画.
//---------------------------------.
void CFont::Render3D( const std::string& text, const bool& isBillboard )
{
	// 文字数分ループ.
	float posX = m_Tranceform.Position.x;
	for( int i = 0; i < static_cast<int>(text.length()); i++ ){
		std::string f = text.substr( i, 1 );
		if( IsDBCSLeadByte( text[i] ) == TRUE ){
			f = text.substr( i++, 2 );	// 全角文字.
		} else {
			f = text.substr( i, 1 );	// 半角文字.
		}
		RenderFont3D( f.c_str(), isBillboard );
	}
}

//---------------------------------.
// テクスチャの描画.
//---------------------------------.
void CFont::RenderFontUI( const char* c )
{
	ID3D11ShaderResourceView* pResourceView = CFontResource::GetTexture2D(c);	// リソースビュー.

	SSpriteRenderState renderState;
	renderState.Tranceform	= m_Tranceform;
	renderState.Color		= m_Color;
	m_pSpriteRender->Render( renderState, pResourceView, m_pVertexBufferUI );
}

//---------------------------------.
// テクスチャの描画.
//---------------------------------.
void CFont::RenderFont3D( const char* c, const bool& isBillboard  )
{
	ID3D11ShaderResourceView* pResourceView = CFontResource::GetTexture2D(c);	// リソースビュー.

	SSpriteRenderState renderState;
	renderState.Tranceform	= m_Tranceform;
	renderState.Color		= m_Color;
	m_pSpriteRender->Render( renderState, pResourceView, m_pVertexBuffer3D );
}

//---------------------------------.
// バーテックスバッファの作成.
//---------------------------------.
HRESULT CFont::CreateVerTexBuffer()
{
	SpriteVertex verticesUI[] =
	{
		D3DXVECTOR3( 0.0f,			 FONT_SIZE*1.2f,	0.0f ),	D3DXVECTOR2( 0.0f, 1.0f ),
		D3DXVECTOR3( 0.0f,			 0.0f,				0.0f ),	D3DXVECTOR2( 0.0f, 0.0f ),
		D3DXVECTOR3( FONT_SIZE*1.0f, FONT_SIZE*1.2f,	0.0f ),	D3DXVECTOR2( 1.0f, 1.0f ),
		D3DXVECTOR3( FONT_SIZE*1.0f, 0.0f,				0.0f ),	D3DXVECTOR2( 1.0f, 0.0f )
	};
	// 最大要素数を算出する.
	UINT uVerMax = sizeof(verticesUI) / sizeof(verticesUI[0]);

	// バッファ構造体.
	D3D11_BUFFER_DESC bd;
	bd.Usage				= D3D11_USAGE_DEFAULT;		// 使用方法.
	bd.ByteWidth			= sizeof(SpriteVertex)*uVerMax;	//頂点のサイズ.
	bd.BindFlags			= D3D11_BIND_VERTEX_BUFFER;	// 頂点バッファとして扱う.
	bd.CPUAccessFlags		= 0;		// CPUからはアクセスしない.
	bd.MiscFlags			= 0;		// その他のフラグ(未使用).
	bd.StructureByteStride	= 0;		// 構造体のサイズ(未使用).

	// サブリソースデータ構造体.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = verticesUI;		// 板ポリの頂点をセット.

	// 頂点バッファの作成.
	if( FAILED( m_pDevice11->CreateBuffer(
		&bd, &InitData, &m_pVertexBufferUI ) ) ){
		_ASSERT_EXPR( false, L"頂点ﾊﾞｯﾌｧ作成失敗" );
		return E_FAIL;
	}

	SpriteVertex vertices[] =
	{
		D3DXVECTOR3(-1.0,  1.0f, 0.0f ),	D3DXVECTOR2( 0.0f, 1.0f ),
		D3DXVECTOR3(-1.0, -1.0f, 0.0f ),	D3DXVECTOR2( 0.0f, 0.0f ),
		D3DXVECTOR3( 1.0,  1.0f, 0.0f ),	D3DXVECTOR2( 1.0f, 1.0f ),
		D3DXVECTOR3( 1.0, -1.0f, 0.0f ),	D3DXVECTOR2( 1.0f, 0.0f )
	};
	// 最大要素数を算出する.
	uVerMax = sizeof(vertices) / sizeof(vertices[0]);
	bd.Usage				= D3D11_USAGE_DEFAULT;		// 使用方法.
	bd.ByteWidth			= sizeof(SpriteVertex)*uVerMax;	//頂点のサイズ.
	bd.BindFlags			= D3D11_BIND_VERTEX_BUFFER;	// 頂点バッファとして扱う.
	bd.CPUAccessFlags		= 0;		// CPUからはアクセスしない.
	bd.MiscFlags			= 0;		// その他のフラグ(未使用).
	bd.StructureByteStride	= 0;		// 構造体のサイズ(未使用).

	InitData.pSysMem = vertices;		// 板ポリの頂点をセット.
	// 頂点バッファの作成.
	if( FAILED( m_pDevice11->CreateBuffer(
		&bd, &InitData, &m_pVertexBuffer3D ) ) ){
		_ASSERT_EXPR( false, L"頂点ﾊﾞｯﾌｧ作成失敗" );
		return E_FAIL;
	}
	return S_OK;
}