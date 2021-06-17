#include "Sprite.h"
#include "..\Shader\Shader.h"
#include "..\..\Utility\FileManager\FileManager.h"
#include "..\..\Object\CameraBase\CameraManager\CameraManager.h"

namespace
{
	const char* SHADER_NAME = "Data\\Shader\\Sprite\\Sprite.hlsl";
	const char* PS_SHADER_ENTRY_NAMES[] =
	{
		"PS_Main",
		"PS_TAlphaOut",
		"PS_TCutOut",
	};
}

CSprite::CSprite()
	: m_SpriteRenderState	()
	, m_pVertexShaderUI		( nullptr )
	, m_pVertexShader3D		( nullptr )
	, m_pPixelShaders		()
	, m_pVertexLayout		( nullptr )
	, m_pConstantBuffer		( nullptr )
	, m_pSampleLinears		()
	, m_pSrcTexture			( nullptr )
	, m_pDestTexture		( nullptr )
	, m_pMaskTexture		( nullptr )
	, m_pVertexBuffer3D		( nullptr )
	, m_pVertexBufferUI		( nullptr )
	, m_SState				()
	, m_Vertices			()
{}

CSprite::~CSprite()
{
	Release();
}

//---------------------------------------.
// 初期化.
//---------------------------------------.
HRESULT CSprite::Init( ID3D11DeviceContext* pContext11, const char* texturePath )
{
	if( FAILED( SetDevice( pContext11 ) ))			return E_FAIL;
	if( FAILED( VertexInitShader( SHADER_NAME ) ))	return E_FAIL;
	if( FAILED( PixelInitShader() ))				return E_FAIL;
	if( FAILED( InitSample() ))						return E_FAIL;
	if( FAILED( InitConstantBuffer() ))				return E_FAIL;
	if( FAILED( CreateTexture( texturePath ) ))		return E_FAIL;
	if( FAILED( SpriteStateRead( texturePath ) ))	return E_FAIL;
	if( FAILED( InitModel3D() ))					return E_FAIL;
	if( FAILED( InitModelUI() ))					return E_FAIL;

	std::string msg = texturePath;
	msg += " 読み込み : 成功";
	CLog::Print( std::string( msg.c_str() ).c_str() );

	return S_OK;
}

//---------------------------------------.
// 解放.
//---------------------------------------.
void CSprite::Release()
{
	SAFE_RELEASE( m_pSrcTexture );
	SAFE_RELEASE( m_pVertexBufferUI );
	SAFE_RELEASE( m_pVertexBuffer3D );

	SAFE_RELEASE( m_pConstantBuffer );
	for( auto& s : m_pSampleLinears )	SAFE_RELEASE( s );
	for( auto& p : m_pPixelShaders )	SAFE_RELEASE( p );
	SAFE_RELEASE( m_pVertexLayout );
	SAFE_RELEASE( m_pVertexShader3D );
	SAFE_RELEASE( m_pVertexShaderUI );

	m_pDestTexture = nullptr;
	m_pMaskTexture = nullptr;

	m_pDevice11 = nullptr;
	m_pContext11 = nullptr;
}

//---------------------------------------.
// 描画.
//---------------------------------------.
void CSprite::Render( SSpriteRenderState* pRenderState )
{
	SSpriteRenderState* renderState = pRenderState == nullptr ? &m_SpriteRenderState : pRenderState;
	SSpriteAnimState* animState = &renderState->AnimState;

	animState->AnimUpdate();		// アニメーションの更新.
	animState->UVScrollUpdate();	// スクロールの更新.

	//　ワールド行列.
	D3DXMATRIX mWorld = renderState->Tranceform.GetWorldMatrix();

	// シェーダーのコンスタントバッファに各種データを渡す.
	D3D11_MAPPED_SUBRESOURCE pData;
	C_BUFFER cb;	// コンスタントバッファ.

	// バッファ内のデータの書き換え開始時にMap.
	if( SUCCEEDED( m_pContext11->Map(
		m_pConstantBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&pData ))){

		cb.mWVP	= mWorld * CCameraManager::GetViewProjMatrix();
		cb.mW	= mWorld;
		D3DXMatrixTranspose( &cb.mWVP, &cb.mWVP );
		D3DXMatrixTranspose( &cb.mW, &cb.mW );

		// ビューポートの幅,高さを渡す.
		cb.vViewPort.x	= static_cast<float>(WND_W);
		cb.vViewPort.y	= static_cast<float>(WND_H);

		// 色を渡す.
		cb.vColor = renderState->Color;

		// フェードの値を渡す.
		cb.fFadeValue = renderState->FadeValue;

		// テクスチャ座標.
		cb.vUV = animState->UV;

		memcpy_s( pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));

		m_pContext11->Unmap( m_pConstantBuffer, 0 );
	}

	// シェーダーの設定.
	m_pContext11->VSSetShader( m_pVertexShaderUI, nullptr, 0 );
	m_pContext11->PSSetShader( m_pPixelShaders[renderState->ShaderNo], nullptr, 0 );

	// コンスタントバッファの設定.
	m_pContext11->VSSetConstantBuffers( 0, 1, &m_pConstantBuffer );
	m_pContext11->PSSetConstantBuffers( 0, 1, &m_pConstantBuffer );

	// 頂点情報の設定.
	UINT stride = sizeof(SpriteVertex);
	UINT offset = 0;
	m_pContext11->IASetVertexBuffers( 0, 1, &m_pVertexBufferUI, &stride, &offset );
	m_pContext11->IASetInputLayout( m_pVertexLayout );

	// 画像情報の設定.
	m_pContext11->PSSetSamplers( 0, 1, &m_pSampleLinears[renderState->SmaplerNo] );
	m_pContext11->PSSetShaderResources( 0, 1, &m_pSrcTexture );
	m_pContext11->PSSetShaderResources( 1, 1, &m_pDestTexture );
	m_pContext11->PSSetShaderResources( 2, 1, &m_pMaskTexture );
	m_pContext11->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

	// 描画.
	SetBlend( true );
	SetDeprh( false );
	SetRasterizerState( ERS_STATE::Back );
	m_pContext11->Draw( 4, 0 );
	SetRasterizerState( ERS_STATE::None );
	SetDeprh( true );
	SetBlend( false );
	m_pDestTexture = nullptr;
	m_pMaskTexture = nullptr;

	ID3D11ShaderResourceView* resetSrv = nullptr;
	m_pContext11->PSSetShaderResources( 0, 1, &resetSrv );
	m_pContext11->PSSetShaderResources( 1, 1, &resetSrv );
	m_pContext11->PSSetShaderResources( 2, 1, &resetSrv );
}

//---------------------------------------.
// 3D描画.
//---------------------------------------.
void CSprite::Render3D( const bool& isBillboard, SSpriteRenderState* pRenderState )
{
	SSpriteRenderState* renderState = pRenderState == nullptr ? &m_SpriteRenderState : pRenderState;
	SSpriteAnimState* animState = &renderState->AnimState;

	animState->AnimUpdate();		// アニメーションの更新.
	animState->UVScrollUpdate();	// スクロールの更新.

	//　ワールド行列.
	D3DXMATRIX mWorld = renderState->Tranceform.GetWorldMatrix();

	if( isBillboard == true ){
		// ビルボード用.
		D3DXMATRIX CancelRotation = CCameraManager::GetViewMatrix();
		CancelRotation._41 = CancelRotation._42 = CancelRotation._43 = 0.0f; // xyzを0にする.
		 // CancelRotationの逆行列を求める.
		D3DXMatrixInverse( &CancelRotation, nullptr, &CancelRotation );
		mWorld = CancelRotation * mWorld;
	}

	// シェーダーのコンスタントバッファに各種データを渡す.
	D3D11_MAPPED_SUBRESOURCE pData;
	C_BUFFER cb;	// コンスタントバッファ.

	// バッファ内のデータの書き換え開始時にMap.
	if( SUCCEEDED( m_pContext11->Map(
		m_pConstantBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&pData ))){

		cb.mWVP	= mWorld * CCameraManager::GetViewProjMatrix();
		cb.mW	= mWorld;
		D3DXMatrixTranspose( &cb.mWVP, &cb.mWVP );
		D3DXMatrixTranspose( &cb.mW, &cb.mW );

		// ビューポートの幅,高さを渡す.
		cb.vViewPort.x	= static_cast<float>(WND_W);
		cb.vViewPort.y	= static_cast<float>(WND_H);

		// 色を渡す.
		cb.vColor = renderState->Color;

		// フェードの値を渡す.
		cb.fFadeValue = renderState->FadeValue;

		// テクスチャ座標.
		cb.vUV = animState->UV;

		memcpy_s( pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));

		m_pContext11->Unmap( m_pConstantBuffer, 0 );
	}

	// シェーダーの設定.
	m_pContext11->VSSetShader( m_pVertexShader3D, nullptr, 0 );
	m_pContext11->PSSetShader( m_pPixelShaders[renderState->ShaderNo], nullptr, 0 );

	// コンスタントバッファの設定.
	m_pContext11->VSSetConstantBuffers( 0, 1, &m_pConstantBuffer );
	m_pContext11->PSSetConstantBuffers( 0, 1, &m_pConstantBuffer );

	// 頂点情報の設定.
	UINT stride = sizeof(SpriteVertex);
	UINT offset = 0;
	m_pContext11->IASetVertexBuffers( 0, 1, &m_pVertexBuffer3D, &stride, &offset );
	m_pContext11->IASetInputLayout( m_pVertexLayout );

	// 画像情報の設定.
	m_pContext11->PSSetSamplers( 0, 1, &m_pSampleLinears[renderState->SmaplerNo] );
	m_pContext11->PSSetShaderResources( 0, 1, &m_pSrcTexture );
	m_pContext11->PSSetShaderResources( 1, 1, &m_pDestTexture );
	m_pContext11->PSSetShaderResources( 2, 1, &m_pMaskTexture );
	m_pContext11->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

	// 描画.
	SetBlend( true );
	m_pContext11->Draw( 4, 0 );
	SetBlend( false );
	m_pDestTexture = nullptr;
	m_pMaskTexture = nullptr;

	ID3D11ShaderResourceView* resetSrv = nullptr;
	m_pContext11->PSSetShaderResources( 0, 1, &resetSrv );
	m_pContext11->PSSetShaderResources( 1, 1, &resetSrv );
	m_pContext11->PSSetShaderResources( 2, 1, &resetSrv );
}

//---------------------------------------.
// シェーダーの作成.
//---------------------------------------.
HRESULT CSprite::VertexInitShader( const char* shaderPath )
{
	ID3DBlob* pCompiledShader = nullptr;
	ID3DBlob* pErrors = nullptr;
	UINT uCompileFlag = 0;
#ifdef _DEBUG
	uCompileFlag =
		D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
#endif	// #ifdef _DEBUG

	// HLSLからバーテックスシェーダーのブロブを作成.
	if( FAILED(
		shader::InitShader(
			shaderPath,			// シェーダーファイル名.
			"VS_MainUI",		// シェーダーエントリーポイント.
			"vs_5_0",			// シェーダーモデル.
			uCompileFlag,		// シェーダーコンパイルフラグ.
			&pCompiledShader,	// ブロブを格納するメモリへのポインタ.
			&pErrors ))) {		// エラーと警告一覧を格納するメモリへのポインタ.
		ERROR_MESSAGE( (char*)pErrors->GetBufferPointer() );
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);

	// 上記で作成したブロブから「頂点シェーダー」を作成.
	if( FAILED( shader::CreateVertexShader(
		m_pDevice11,
		pCompiledShader, 
		&m_pVertexShaderUI ))) {
		ERROR_MESSAGE("頂点シェーダー作成 : 失敗");
		return E_FAIL;
	}

	// HLSLからバーテックスシェーダーのブロブを作成.
	if( FAILED(
		shader::InitShader(
			shaderPath,			// シェーダーファイル名.
			"VS_Main",			// シェーダーエントリーポイント.
			"vs_5_0",			// シェーダーモデル.
			uCompileFlag,		// シェーダーコンパイルフラグ.
			&pCompiledShader,	// ブロブを格納するメモリへのポインタ.
			&pErrors ))) {		// エラーと警告一覧を格納するメモリへのポインタ.
		ERROR_MESSAGE( (char*)pErrors->GetBufferPointer() );
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);

	// 上記で作成したブロブから「頂点シェーダー」を作成.
	if( FAILED( shader::CreateVertexShader(
		m_pDevice11,
		pCompiledShader, 
		&m_pVertexShader3D ))) {
		ERROR_MESSAGE("頂点シェーダー作成 : 失敗");
		return E_FAIL;
	}

	// 頂点インプットレイアウトを定義.
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		shader::GetPositionInputElement(),
		shader::GetTexcoordInputElement(),
	};

	// 頂点インプットレイアウトの配列要素数を算出.
	UINT numElements = sizeof(layout) / sizeof(layout[0]);

	// 頂点インプットレイアウトを作成.
	if( FAILED(
		shader::CreateInputLayout(
			m_pDevice11,
			layout,
			numElements,
			pCompiledShader,
			&m_pVertexLayout))) {
		ERROR_MESSAGE("頂点インプットレイアウト作成 : 失敗");
		return E_FAIL;
	}
	SAFE_RELEASE(pCompiledShader);

	return S_OK;
}

//---------------------------------------.
// ピクセルシェーダーの作成.
//---------------------------------------.
HRESULT CSprite::PixelInitShader()
{
	ID3DBlob* pCompiledShader = nullptr;
	ID3DBlob* pErrors = nullptr;
	UINT uCompileFlag = 0;
#ifdef _DEBUG
	uCompileFlag =
		D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
#endif	// #ifdef _DEBUG
	int i = 0;
	m_pPixelShaders.resize(EPSShaderNo_Max);
	for( auto& p : m_pPixelShaders ){
		// HLSLからピクセルシェーダーのブロブを作成.
		if( FAILED(
			shader::InitShader(
				SHADER_NAME,				// シェーダーファイル名.
				PS_SHADER_ENTRY_NAMES[i],	// シェーダーエントリーポイント.
				"ps_5_0",					// シェーダーモデル.
				uCompileFlag,				// シェーダーコンパイルフラグ.
				&pCompiledShader,			// ブロブを格納するメモリへのポインタ.
				&pErrors ))) {				// エラーと警告一覧を格納するメモリへのポインタ.
			ERROR_MESSAGE( (char*)pErrors->GetBufferPointer() );
			return E_FAIL;
		}
		SAFE_RELEASE(pErrors);

		// 上記で作成したブロブから「ピクセルシェーダー」を作成.
		if( FAILED( shader::CreatePixelShader(
			m_pDevice11,
			pCompiledShader, 
			&m_pPixelShaders[i] ))) {
			ERROR_MESSAGE("ピクセルシェーダー作成 : 失敗");
			return E_FAIL;
		}
		i++;
	}
	return S_OK;
}

//---------------------------------------.
// 定数バッファの作成.
//---------------------------------------.
HRESULT CSprite::InitConstantBuffer()
{
	if( FAILED(
		shader::CreateConstantBuffer( m_pDevice11, &m_pConstantBuffer, sizeof(C_BUFFER) ))) {
		ERROR_MESSAGE("定数バッファ作成 : 失敗");
		return E_FAIL;
	}
	return S_OK;
}

//---------------------------------------.
// サンプラの作成.
//---------------------------------------.
HRESULT CSprite::InitSample()
{
	// テクスチャ用のサンプラ構造体.
	D3D11_SAMPLER_DESC samDesc;
	ZeroMemory( &samDesc, sizeof(samDesc) );
	samDesc.Filter		= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samDesc.AddressU	= D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.AddressV	= D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.AddressW	= D3D11_TEXTURE_ADDRESS_WRAP;

	// サンプラ作成.
	for( int i = 0; i < ESamplerState_Max; i++ ){
		samDesc.AddressU = static_cast<D3D11_TEXTURE_ADDRESS_MODE>(i+1);
		samDesc.AddressV = static_cast<D3D11_TEXTURE_ADDRESS_MODE>(i+1);
		samDesc.AddressW = static_cast<D3D11_TEXTURE_ADDRESS_MODE>(i+1);
		if( FAILED( m_pDevice11->CreateSamplerState( &samDesc, &m_pSampleLinears[i] ))){
			ERROR_MESSAGE("サンプラ作成 : 失敗");
			return E_FAIL;
		}
	}

	return S_OK;
}

//---------------------------------------.
// スプライト情報の読み込み.
//---------------------------------------.
HRESULT CSprite::SpriteStateRead( const char* filePath )
{
	std::string path = filePath;
	// 拡張子の"."の位置を取得.
	size_t i = path.find(".");
	// 拡張子取り除いて".txt"に変更.
	path.erase( i, path.size() ) += ".txt";

	std::vector<std::string> pramList = fileManager::TextLoading( path );

	if( pramList.empty() == true ){
		ERROR_MESSAGE( path + "が存在しません" );
		return E_FAIL;
	}

	m_SState.LocalPosNum	= static_cast<ELocalPosition>(std::stoi(pramList[ESpriteStateNo_LocalPosNum]));
	m_SState.Disp.w			= std::stof(pramList[ESpriteStateNo_Disp_w]);
	m_SState.Disp.h			= std::stof(pramList[ESpriteStateNo_Disp_h]);
	m_SState.Base.w			= std::stof(pramList[ESpriteStateNo_Base_w]);
	m_SState.Base.h			= std::stof(pramList[ESpriteStateNo_Base_h]);
	m_SState.Stride.w		= std::stof(pramList[ESpriteStateNo_Stride_w]);
	m_SState.Stride.h		= std::stof(pramList[ESpriteStateNo_Stride_h]);
	m_SState.vPos.x			= std::stof(pramList[ESpriteStateNo_vPos_x]);
	m_SState.vPos.y			= std::stof(pramList[ESpriteStateNo_vPos_y]);
	m_SState.vPos.z			= std::stof(pramList[ESpriteStateNo_vPos_z]);
	m_SState.AnimNum		= std::stoi(pramList[ESpriteStateNo_AnimNum]);

	m_SpriteRenderState.AnimState.MaxAnimNumber = m_SState.AnimNum;
	m_SpriteRenderState.Tranceform.Position = m_SState.vPos;

	return S_OK;
}

//---------------------------------------.
// テクスチャ作成.
//---------------------------------------.
HRESULT CSprite::CreateTexture( const char* texturePath )
{
	if( FAILED( D3DX11CreateShaderResourceViewFromFile(
		m_pDevice11,	// リソースを使用するデバイスのポインタ.
		texturePath,	// テクスチャパス名.
		nullptr,
		nullptr,
		&m_pSrcTexture,		// (out)テクスチャ.
		nullptr ))){
		std::string err = texturePath;
		err += " : テクスチャ読み込み : 失敗";
		ERROR_MESSAGE( err );
		return E_FAIL;
	}
	return S_OK;
}

//---------------------------------------.
// モデル作成 3D 用.
//---------------------------------------.
HRESULT CSprite::InitModel3D()
{
	// 画像の比率を求める.
	int as = myGcd((int)m_SState.Disp.w, (int)m_SState.Disp.h);
	float w = m_SState.Disp.w / as;
	float h = m_SState.Disp.h / as;
	w *= 0.1f;
	h *= 0.1f;
	float u = m_SState.Stride.w / m_SState.Base.w;	// 1コマ当たりの幅,
	float v = m_SState.Stride.h / m_SState.Base.h;	// 1コマ当たりの高さ.

	CreateVertex( w, -h, u, v, true );

	// サブリソース構造体.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = m_Vertices;

	// バッファ構造体.
	D3D11_BUFFER_DESC bd;
	bd.Usage				= D3D11_USAGE_DEFAULT;		// 使用方法(デフォルト).
	bd.ByteWidth			= sizeof(SpriteVertex) * 4;		// 頂点のサイズ.
	bd.BindFlags			= D3D11_BIND_VERTEX_BUFFER;	// 頂点バッファとして扱う.
	bd.CPUAccessFlags		= 0;						// CPUからはアクセスしない.
	bd.MiscFlags			= 0;						// その他のフラグ(未使用).
	bd.StructureByteStride	= 0;						// 構造体のサイズ(未使用).

	// 頂点バッファの作成.
	if( FAILED( m_pDevice11->CreateBuffer( &bd, &InitData, &m_pVertexBuffer3D ))){
		ERROR_MESSAGE( "頂点バッファ作成 : 失敗" );
		return E_FAIL;
	}

	return S_OK;
}

//---------------------------------------.
// モデル作成 UI 用.
//---------------------------------------.
HRESULT CSprite::InitModelUI()
{
	float w = m_SState.Disp.w;	// 表示スプライト幅,
	float h = m_SState.Disp.h;	// 表示スプライト高さ,
	float u = m_SState.Stride.w / m_SState.Base.w;	// 1コマ当たりの幅,
	float v = m_SState.Stride.h / m_SState.Base.h;	// 1コマ当たりの高さ.

	m_SpriteRenderState.AnimState.PatternMax.x = static_cast<LONG>( m_SState.Base.w / m_SState.Stride.w );// xの最大マス数.
	m_SpriteRenderState.AnimState.PatternMax.y = static_cast<LONG>( m_SState.Base.h / m_SState.Stride.h );// yの最大マス数.
	//-------------------------------.
	// UI用.
	//-------------------------------.
	// バッファ構造体.
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;				// 使用方法(デフォルト).
	bd.ByteWidth = sizeof(SpriteVertex) * 4;	// 頂点のサイズ.
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// 頂点バッファとして扱う.
	bd.CPUAccessFlags = 0;						// CPUからはアクセスしない.
	bd.MiscFlags = 0;							// その他のフラグ(未使用).
	bd.StructureByteStride = 0;					// 構造体のサイズ(未使用).

	// サブリソース構造体.
	D3D11_SUBRESOURCE_DATA InitData;
	CreateVertex( w, h, u, v, false );
	InitData.pSysMem = m_Vertices;	// 板ポリの頂点をセット.

	// 頂点バッファの作成.
	if( FAILED( 
		m_pDevice11->CreateBuffer( &bd, &InitData, &m_pVertexBufferUI ) )) {
		ERROR_MESSAGE( "頂点バッファ作成 : 失敗" );
		return E_FAIL;
	}
	return S_OK;
}

//---------------------------------------.
// 頂点情報の作成.
//---------------------------------------.
void CSprite::CreateVertex( const float& w, const float& h, const float& u, const float& v, const bool& is3D )
{
	const float minusH = h * -1.0f;
	switch( m_SState.LocalPosNum )
	{
	case ELocalPosition::LeftUp:
		m_Vertices[0] =
		{ D3DXVECTOR3( 0.0f,	h,		0.0f ), D3DXVECTOR2( 0.0f,	v ) };		// 頂点１(左下).
		m_Vertices[1] =
		{ D3DXVECTOR3( 0.0f,	0.0f,	0.0f ), D3DXVECTOR2( 0.0f,	0.0f ) };	// 頂点２(左上).
		m_Vertices[2] =
		{ D3DXVECTOR3( w,		h,		0.0f ), D3DXVECTOR2( u,		v ) };		// 頂点３(右下).
		m_Vertices[3] =
		{ D3DXVECTOR3( w,		0.0f,	0.0f ), D3DXVECTOR2( u,		0.0f ) };	// 頂点４(右上).
		break;
	case ELocalPosition::Left:
		m_Vertices[0] =
		{ D3DXVECTOR3( 0.0f,	h / 2,		0.0f ), D3DXVECTOR2( 0.0f,	v ) };		// 頂点１(左下).
		m_Vertices[1] =
		{ D3DXVECTOR3( 0.0f,	minusH / 2,	0.0f ), D3DXVECTOR2( 0.0f,	0.0f ) };	// 頂点２(左上).
		m_Vertices[2] =
		{ D3DXVECTOR3( w,		h / 2,		0.0f ), D3DXVECTOR2( u,		v ) };		// 頂点３(右下).
		m_Vertices[3] =
		{ D3DXVECTOR3( w,		minusH / 2,	0.0f ), D3DXVECTOR2( u,		0.0f ) };	// 頂点４(右上).
		break;
	case ELocalPosition::LeftDown:
		m_Vertices[0] =
		{ D3DXVECTOR3( 0.0f,	0.0f,		0.0f ), D3DXVECTOR2( 0.0f,	v ) };		// 頂点１(左下).
		m_Vertices[1] =
		{ D3DXVECTOR3( 0.0f,	minusH,		0.0f ), D3DXVECTOR2( 0.0f,	0.0f ) };	// 頂点２(左上).
		m_Vertices[2] =
		{ D3DXVECTOR3( w,		0.0f,		0.0f ), D3DXVECTOR2( u,		v ) };		// 頂点３(右下).
		m_Vertices[3] =
		{ D3DXVECTOR3( w,		minusH,		0.0f ), D3DXVECTOR2( u,		0.0f ) };	// 頂点４(右上).
		break;
	case ELocalPosition::Down:
		m_Vertices[0] =
		{ D3DXVECTOR3(-w / 2,	0.0f,		0.0f ), D3DXVECTOR2( 0.0f,	v ) };		// 頂点１(左下).
		m_Vertices[1] =
		{ D3DXVECTOR3(-w / 2,	minusH,		0.0f ), D3DXVECTOR2( 0.0f,	0.0f ) };	// 頂点２(左上).
		m_Vertices[2] =
		{ D3DXVECTOR3( w / 2,	0.0f,		0.0f ), D3DXVECTOR2( u,		v ) };		// 頂点３(右下).
		m_Vertices[3] =
		{ D3DXVECTOR3( w / 2,	minusH,		0.0f ), D3DXVECTOR2( u,		0.0f ) };	// 頂点４(右上).
		break;
	case ELocalPosition::RightDown:
		m_Vertices[0] =
		{ D3DXVECTOR3(-w,		0.0f,		0.0f ), D3DXVECTOR2( 0.0f,	v ) };		// 頂点１(左下).
		m_Vertices[1] =
		{ D3DXVECTOR3(-w,		minusH,		0.0f ), D3DXVECTOR2( 0.0f,	0.0f ) };	// 頂点２(左上).
		m_Vertices[2] =
		{ D3DXVECTOR3( 0.0f,	0.0f,		0.0f ), D3DXVECTOR2( u,		v ) };		// 頂点３(右下).
		m_Vertices[3] =
		{ D3DXVECTOR3( 0.0f,	minusH,		0.0f ), D3DXVECTOR2( u,		0.0f ) };	// 頂点４(右上).
		break;
	case ELocalPosition::Right:
		m_Vertices[0] =
		{ D3DXVECTOR3(-w,		h / 2,		0.0f ), D3DXVECTOR2( 0.0f,	v ) };		// 頂点１(左下).
		m_Vertices[1] =
		{ D3DXVECTOR3(-w,		minusH / 2,	0.0f ), D3DXVECTOR2( 0.0f,	0.0f ) };	// 頂点２(左上).
		m_Vertices[2] =
		{ D3DXVECTOR3( 0.0f,	h / 2,		0.0f ), D3DXVECTOR2( u,		v ) };		// 頂点３(右下).
		m_Vertices[3] =
		{ D3DXVECTOR3( 0.0f,	minusH / 2,	0.0f ), D3DXVECTOR2( u,		0.0f ) };	// 頂点４(右上).
		break;
	case ELocalPosition::RightUp:
		m_Vertices[0] =
		{ D3DXVECTOR3(-w,		h,		0.0f ), D3DXVECTOR2( 0.0f,	v ) };		// 頂点１(左下).
		m_Vertices[1] =
		{ D3DXVECTOR3(-w,		0.0f,	0.0f ), D3DXVECTOR2( 0.0f,	0.0f ) };	// 頂点２(左上).
		m_Vertices[2] =
		{ D3DXVECTOR3( 0.0f,	h,		0.0f ), D3DXVECTOR2( u,		v ) };		// 頂点３(右下).
		m_Vertices[3] =
		{ D3DXVECTOR3( 0.0f,	0.0f,	0.0f ), D3DXVECTOR2( u,		0.0f ) };	// 頂点４(右上).
		break;
	case ELocalPosition::Up:
		m_Vertices[0] =
		{ D3DXVECTOR3(-w / 2,	h,		0.0f ), D3DXVECTOR2( 0.0f,	v) };		// 頂点１(左下).
		m_Vertices[1] =
		{ D3DXVECTOR3(-w / 2,	0.0f,	0.0f ), D3DXVECTOR2( 0.0f,	0.0f) };	// 頂点２(左上).
		m_Vertices[2] =
		{ D3DXVECTOR3( w / 2,	h,		0.0f ), D3DXVECTOR2( u,		v) };		// 頂点３(右下).
		m_Vertices[3] =
		{ D3DXVECTOR3( w / 2,	0.0f,	0.0f ), D3DXVECTOR2( u,		0.0f) };	// 頂点４(右上).
		break;
	case ELocalPosition::Center:
	default:
		m_Vertices[0] =
		{ D3DXVECTOR3(-w / 2,	h / 2,		0.0f ), D3DXVECTOR2( 0.0f,	v ) };		// 頂点１(左下).
		m_Vertices[1] =
		{ D3DXVECTOR3(-w / 2,	minusH / 2,	0.0f ), D3DXVECTOR2( 0.0f,	0.0f ) };	// 頂点２(左上).
		m_Vertices[2] =
		{ D3DXVECTOR3( w / 2,	h / 2,		0.0f ), D3DXVECTOR2( u,		v ) };		// 頂点３(右下).
		m_Vertices[3] =
		{ D3DXVECTOR3( w / 2,	minusH / 2,	0.0f ), D3DXVECTOR2( u,		0.0f ) };	// 頂点４(右上).
		break;
	}
}