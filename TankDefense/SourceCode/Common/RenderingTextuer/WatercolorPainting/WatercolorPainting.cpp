#include "WatercolorPainting.h"
#include "..\..\Shader\Shader.h"
#include "..\..\..\Object\CameraBase\CameraManager\CameraManager.h"
#include "..\..\..\Object\LightBase\LightManager\LightManager.h"
#include "..\..\..\Resource\SpriteResource\SpriteResource.h"

namespace
{
	const char*	SHADER_NAME			= "Data\\Shader\\RenderTexture\\WatercolorPainting\\WatercolorPainting_PS.hlsl";	// ピクセルシェーダー名.
	const char*	SHADER_ENTRY_NAME	= "PS_Main";	// ピクセルシェーダーエントリー名.
	const int	BUFFER_COUNT_MAX	= 1;			// バッファーの最大数.
	const int	BUFFER_INDEX_COUNT	= 0;			// バッファーの配列の番号.
};

CWatercolorPaintingRender::CWatercolorPaintingRender()
	: m_pConstantBufferFrame	( nullptr )
{
}

CWatercolorPaintingRender::~CWatercolorPaintingRender()
{
}

//------------------------------------.
// 初期化.
//------------------------------------.
HRESULT CWatercolorPaintingRender::Init( ID3D11DeviceContext* pContext11 )
{
	if( FAILED( InitBase( pContext11 ) ))		return E_FAIL;
	if( FAILED( InitBufferTex() ))				return E_FAIL;
	if( FAILED( InitPixelShader( SHADER_NAME, SHADER_ENTRY_NAME ) )) return E_FAIL;
	if( FAILED( shader::CreateConstantBuffer( m_pDevice11, &m_pConstantBufferFrame, sizeof(LIGHT_CBUFFER) ))) return E_FAIL;

	return S_OK;
}

//------------------------------------.
// 描画.
//------------------------------------.
void CWatercolorPaintingRender::Render( const int& srvCount, const std::vector<ID3D11ShaderResourceView*>& srvList )
{
	CWatercolorPaintingRender::SetBuffer();

	// シェーダーのコンスタントバッファに各種データを渡す.
	D3D11_MAPPED_SUBRESOURCE pData;
	if( SUCCEEDED( m_pContext11->Map( m_pConstantBufferFrame, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData ))){
		LIGHT_CBUFFER cb;


		// メモリ領域をコピー.
		memcpy_s( pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb) );
		m_pContext11->Unmap( m_pConstantBufferFrame, 0 );
	}

	// 使用するシェーダのセット.
	m_pContext11->VSSetShader( m_pVertexShader, nullptr, 0 );	// 頂点シェーダ.
	m_pContext11->PSSetShader( m_pPixelShader, nullptr, 0 );	// ピクセルシェーダ.
	m_pContext11->PSSetSamplers( 0, 1, &m_pSampleLinear );		// サンプラのセット.

	m_pContext11->VSSetConstantBuffers( 0, 1, &m_pConstantBufferInit );	// 頂点シェーダ.
	m_pContext11->PSSetConstantBuffers( 0, 1, &m_pConstantBufferInit );	// ピクセルシェーダー.
	m_pContext11->VSSetConstantBuffers( 1, 1, &m_pConstantBufferFrame );	// 頂点シェーダ.
	m_pContext11->PSSetConstantBuffers( 1, 1, &m_pConstantBufferFrame );	// ピクセルシェーダー.

	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	m_pContext11->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, &stride, &offset );
	m_pContext11->IASetInputLayout( m_pVertexLayout );
	m_pContext11->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
	
	ID3D11ShaderResourceView* pNormTex = CSpriteResource::GetSprite( "norm" )->GetTexture();
	ID3D11ShaderResourceView* pNoiseTex = CSpriteResource::GetSprite( "noise" )->GetTexture();
	m_pContext11->PSSetShaderResources( 0, 1, &pNormTex );
	m_pContext11->PSSetShaderResources( 1, 1, &pNoiseTex );
	m_pContext11->PSSetShaderResources( 2, srvCount, &srvList[0] );
	SetRasterizerState( ERS_STATE::Back );
	m_pContext11->Draw( 4, 0 );
	SetRasterizerState( ERS_STATE::None );
}

//------------------------------------.
// バッファの設定.
//------------------------------------.
void CWatercolorPaintingRender::SetBuffer()
{
	CRenderTexture::SetBuffer( BUFFER_COUNT_MAX );
}

//------------------------------------.
// テクスチャの初期化.
//------------------------------------.
HRESULT CWatercolorPaintingRender::InitBufferTex()
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

	m_pRenderTargetViewList.resize( BUFFER_COUNT_MAX );
	m_pShaderResourceViewList.resize( BUFFER_COUNT_MAX );
	m_pTexture2DList.resize( BUFFER_COUNT_MAX );

	if( FAILED( CreateBufferTex(
		texDesc,
		&m_pRenderTargetViewList[BUFFER_INDEX_COUNT],
		&m_pShaderResourceViewList[BUFFER_INDEX_COUNT],
		&m_pTexture2DList[BUFFER_INDEX_COUNT] ))) return E_FAIL;

	return S_OK;
}