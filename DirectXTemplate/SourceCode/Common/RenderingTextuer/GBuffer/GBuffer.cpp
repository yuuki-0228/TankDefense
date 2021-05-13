#include "GBuffer.h"

CGBufferRender::CGBufferRender()
{
}

CGBufferRender::~CGBufferRender()
{
}

//------------------------------------.
// 初期化.
//------------------------------------.
HRESULT CGBufferRender::Init( ID3D11DeviceContext* pContext11 )
{
	if( FAILED( InitBase( pContext11 ) ))		return E_FAIL;
	if( FAILED( InitBufferTex() ))				return E_FAIL;
	return S_OK;
}

//------------------------------------.
// バッファの設定.
//------------------------------------.
void CGBufferRender::SetBuffer()
{
	CRenderTexture::SetBuffer( EGBufferNo_MAX );
}

//------------------------------------.
// テクスチャの初期化.
//------------------------------------.
HRESULT CGBufferRender::InitBufferTex()
{
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width				= m_WndWidth ;						// 幅.
	texDesc.Height				= m_WndHeight;						// 高さ.
	texDesc.MipLevels			= 1;								// ミップマップレベル:1.
	texDesc.ArraySize			= 1;								// 配列数:1.
	texDesc.Format				= DXGI_FORMAT_R16G16B16A16_FLOAT;	// 32ビットフォーマット.
	texDesc.SampleDesc.Count	= 1;								// マルチサンプルの数.
	texDesc.SampleDesc.Quality	= 0;								// マルチサンプルのクオリティ.
	texDesc.Usage				= D3D11_USAGE_DEFAULT;				// 使用方法:デフォルト.
	texDesc.BindFlags			= D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;	// レンダーターゲット、シェーダーリソース.
	texDesc.CPUAccessFlags		= 0;								// CPUからはアクセスしない.
	texDesc.MiscFlags			= 0;								// その他の設定なし.

	m_pRenderTargetViewList.resize( EGBufferNo_MAX );
	m_pShaderResourceViewList.resize( EGBufferNo_MAX );
	m_pTexture2DList.resize( EGBufferNo_MAX );

	for( int i = 0; i < EGBufferNo_MAX; i++ ){
		if( i == EGBufferNo_Z_DEPTH ){
			texDesc.Format = DXGI_FORMAT_R32_FLOAT;;
		} else {
			texDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		}
		if( FAILED( CreateBufferTex(
			texDesc,
			&m_pRenderTargetViewList[i],
			&m_pShaderResourceViewList[i],
			&m_pTexture2DList[i] ))) return E_FAIL;
	}
	return S_OK;
}
