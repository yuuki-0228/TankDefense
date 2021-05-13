#include "RenderingTextuer.h"
#include "..\Shader\Shader.h"
#include "..\D3DX\D3DX11.h"

namespace
{
	const char* SHADER_NAME			= "Data\\Shader\\RenderTexture\\RenderTexture_VS.hlsl";
	const char* SHADER_ENTRY_NAME	= "VS_Main";
};

CRenderTexture::CRenderTexture()
	: m_pRenderTargetViewList	()
	, m_pShaderResourceViewList	()
	, m_pTexture2DList			()
	, m_pVertexShader			( nullptr )
	, m_pPixelShader			( nullptr )
	, m_pVertexLayout			( nullptr )
	, m_pConstantBufferInit		( nullptr )
	, m_pVertexBuffer			( nullptr )
	, m_pSampleLinear			( nullptr )
	, m_WndWidth				( 1 )
	, m_WndHeight				( 1 )
{
}

CRenderTexture::~CRenderTexture()
{
}

// 初期化.
HRESULT CRenderTexture::InitBase( ID3D11DeviceContext* pContext11 )
{
	if( m_pDevice11 == nullptr ){
		if( FAILED( SetDevice( pContext11 ) )) return E_FAIL;
	}

	// ウィンドウサイズの取得.
	m_WndWidth	= CDirectX11::GetWndWidth();
	m_WndHeight	= CDirectX11::GetWndHeight();

	if( FAILED( InitVertexShader( SHADER_NAME, SHADER_ENTRY_NAME )) )	return E_FAIL;
	if( FAILED( InitVertexBuffer() ))	return E_FAIL;
	if( FAILED( InitConstantBuffer()) )	return E_FAIL;
	if( FAILED( InitSampleLinear() ))	return E_FAIL;
	return S_OK;
}

// 頂点バッファーの作成.
HRESULT CRenderTexture::InitVertexBuffer()
{
	const float wnd_w = static_cast<float>(m_WndWidth);
	const float wnd_h = static_cast<float>(m_WndHeight);
	// 板ポリ(四角形)の頂点を作成.
	VERTEX vertices[]=
	{
		// 頂点座標(x,y,z)				 
		D3DXVECTOR3( 0.0f,	wnd_h,	0.0f ),	D3DXVECTOR2( 0.0f, 1.0f ),
		D3DXVECTOR3( 0.0f,	0.0f,	0.0f ),	D3DXVECTOR2( 0.0f, 0.0f ),
		D3DXVECTOR3( wnd_w,	wnd_h,	0.0f ),	D3DXVECTOR2( 1.0f, 1.0f ),
		D3DXVECTOR3( wnd_w,	0.0f,	0.0f ),	D3DXVECTOR2( 1.0f, 0.0f ),
	};
	// 最大要素数を算出する.
	UINT uVerMax = sizeof(vertices) / sizeof(vertices[0]);

	// バッファ構造体.
	D3D11_BUFFER_DESC bd;
	bd.Usage				= D3D11_USAGE_DEFAULT;		// 使用方法(デフォルト).
	bd.ByteWidth			= sizeof(VERTEX) * uVerMax;	// 頂点のサイズ.
	bd.BindFlags			= D3D11_BIND_VERTEX_BUFFER;	// 頂点バッファとして扱う.
	bd.CPUAccessFlags		= 0;	// CPUからはアクセスしない.
	bd.MiscFlags			= 0;	// その他のフラグ(未使用).
	bd.StructureByteStride	= 0;	// 構造体のサイズ(未使用).

	// サブリソースデータ構造体.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;	// 板ポリの頂点をセット.

	// 頂点バッファの作成.
	if( FAILED( m_pDevice11->CreateBuffer(
		&bd, 
		&InitData, 
		&m_pVertexBuffer))){
		ERROR_MESSAGE( "頂点ﾊﾞｯﾌｧ作成失敗" );
		return E_FAIL;
	}

	return S_OK;
}

// コンスタントバッファの作成.
HRESULT CRenderTexture::InitConstantBuffer()
{
	shader::CreateConstantBuffer( m_pDevice11, &m_pConstantBufferInit, sizeof(C_BUFFER_PER_INIT) );

	// シェーダーのコンスタントバッファに各種データを渡す.
	D3D11_MAPPED_SUBRESOURCE pData;

	// バッファ内のデータの書き換え開始時にMap.
	if( FAILED( m_pContext11->Map( m_pConstantBufferInit, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData ))){
		ERROR_MESSAGE( "コンスタントバッファ書き換え : 失敗" );
		return E_FAIL;
	}

	C_BUFFER_PER_INIT cb;	// コンスタントバッファ.
	D3DXMatrixIdentity( &cb.mW );
	D3DXMatrixTranspose( &cb.mW, &cb.mW );
	// ビューポートの幅,高さを渡す.
	cb.vViewPort.x = static_cast<float>(m_WndWidth);
	cb.vViewPort.y = static_cast<float>(m_WndHeight);

	cb.vPixelSize.x = 1.0f / static_cast<float>(m_WndWidth);
	cb.vPixelSize.y = 1.0f / static_cast<float>(m_WndHeight);

	// メモリ領域をコピー.
	memcpy_s( pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb) );
	m_pContext11->Unmap( m_pConstantBufferInit, 0 );

	return S_OK;
}

// 解放.
void CRenderTexture::Release()
{
	for( auto& r : m_pRenderTargetViewList )	SAFE_RELEASE( r );
	for( auto& s : m_pShaderResourceViewList )	SAFE_RELEASE( s );
	for( auto& t : m_pTexture2DList )			SAFE_RELEASE( t );

	SAFE_RELEASE( m_pVertexShader );
	SAFE_RELEASE( m_pPixelShader );
	SAFE_RELEASE( m_pVertexLayout );
	SAFE_RELEASE( m_pConstantBufferInit );
	SAFE_RELEASE( m_pVertexBuffer );

	m_pContext11 = nullptr;
	m_pDevice11 = nullptr;
}

// バッファの設定.
void CRenderTexture::SetBuffer( const int& numViews )
{
	// G-Bufferテクスチャのクリア.
	for( auto& rtv : m_pRenderTargetViewList ){
		m_pContext11->ClearRenderTargetView( rtv, CLEAR_BACK_COLOR );
	}
	// レンダーターゲットの設定.
	m_pContext11->OMSetRenderTargets( 
		numViews,
		&m_pRenderTargetViewList[0],
		CDirectX11::GetDepthSV() );
	// デプスステンシルバッファ.
	m_pContext11->ClearDepthStencilView(
		CDirectX11::GetDepthSV(), D3D11_CLEAR_DEPTH, 1.0f, 0 );
}

// 頂点シェーダー作成.
HRESULT CRenderTexture::InitVertexShader( const char* filePath, const char* entryName )
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
			filePath,			// シェーダーファイル名.
			entryName,			// シェーダーエントリーポイント.
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
		&m_pVertexShader ))) {
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

// ピクセルシェーダー作成.
HRESULT CRenderTexture::InitPixelShader( const char* filePath, const char* entryName )
{
	ID3DBlob* pCompiledShader = nullptr;
	ID3DBlob* pErrors = nullptr;
	UINT uCompileFlag = 0;
#ifdef _DEBUG
	uCompileFlag =
		D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
#endif	// #ifdef _DEBUG

	// HLSLからピクセルシェーダーのブロブを作成.
	if( FAILED(
		shader::InitShader(
			filePath,			// シェーダーファイル名.
			entryName,			// シェーダーエントリーポイント.
			"ps_5_0",			// シェーダーモデル.
			uCompileFlag,		// シェーダーコンパイルフラグ.
			&pCompiledShader,	// ブロブを格納するメモリへのポインタ.
			&pErrors ))) {		// エラーと警告一覧を格納するメモリへのポインタ.
		ERROR_MESSAGE( (char*)pErrors->GetBufferPointer() );
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);

	// 上記で作成したブロブから「ピクセルシェーダー」を作成.
	if( FAILED( shader::CreatePixelShader(
		m_pDevice11,
		pCompiledShader, 
		&m_pPixelShader ))) {
		ERROR_MESSAGE("ピクセルシェーダー作成 : 失敗");
		return E_FAIL;
	}
	return S_OK;
}
// サンプラーの作成.
HRESULT CRenderTexture::InitSampleLinear()
{
	// テクスチャ用のサンプラ構造体.
	D3D11_SAMPLER_DESC samDesc;
	ZeroMemory( &samDesc, sizeof( samDesc ) );
	samDesc.Filter			= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samDesc.AddressU		= D3D11_TEXTURE_ADDRESS_CLAMP;		// UV値が[0,1]を超えたら,[0,1]に設定する.
	samDesc.AddressV		= D3D11_TEXTURE_ADDRESS_CLAMP;
	samDesc.AddressW		= D3D11_TEXTURE_ADDRESS_CLAMP;
	samDesc.ComparisonFunc	= D3D11_COMPARISON_NEVER;
	samDesc.MinLOD			= 0;
	samDesc.MaxLOD			= D3D11_FLOAT32_MAX;
	// サンプラ作成.
	if( FAILED( m_pDevice11->CreateSamplerState(
		&samDesc, &m_pSampleLinear ) ) ){
		ERROR_MESSAGE( "SamplerState creation failed" );
		return E_FAIL;
	}
	return S_OK;
}

// バッファの作成.
HRESULT CRenderTexture::CreateBufferTex(
	const D3D11_TEXTURE2D_DESC	texDesc,
	ID3D11RenderTargetView**	ppRTV,
	ID3D11ShaderResourceView**	ppSRV,
	ID3D11Texture2D**			ppTex )
{
	// そのテクスチャに対してデプスステンシル(DSTex)を作成.
	if( FAILED( m_pDevice11->CreateTexture2D( &texDesc, nullptr, ppTex )) ){
		_ASSERT_EXPR( false, L"テクスチャデスク作成失敗" );
		return E_FAIL;
	}
	// レンダーターゲットビューの設定
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	memset( &rtvDesc, 0, sizeof( rtvDesc ) );
	rtvDesc.Format             = texDesc.Format;
	rtvDesc.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;
	// RenderTargetView作成.
	if( FAILED( m_pDevice11->CreateRenderTargetView( *ppTex, &rtvDesc, ppRTV ) )){
		_ASSERT_EXPR( false, L"RenderTargetView作成失敗" );
		return E_FAIL;
	}

	// シェーダリソースビューの設定
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	memset( &srvDesc, 0, sizeof( srvDesc ) );
	srvDesc.Format              = rtvDesc.Format;
	srvDesc.ViewDimension       = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	// テクスチャ作成時と同じフォーマット
	if( FAILED( m_pDevice11->CreateShaderResourceView( *ppTex, &srvDesc, ppSRV ) )){
		_ASSERT_EXPR( false, L"デプスステンシル作成失敗" );
		return E_FAIL;
	}
	return S_OK;
}
