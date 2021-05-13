#include "Lighting.h"
#include "..\..\Shader\Shader.h"
#include "..\..\..\Object\CameraBase\CameraManager\CameraManager.h"
#include "..\..\..\Object\LightBase\LightManager\LightManager.h"

namespace
{
	const char*	SHADER_NAME			= "Data\\Shader\\RenderTexture\\Lighting\\Lighting_PS.hlsl";	// ピクセルシェーダー名.
	const char*	SHADER_ENTRY_NAME	= "PS_Main";	// ピクセルシェーダーエントリー名.
	const int	BUFFER_COUNT_MAX	= 1;			// バッファーの最大数.
	const int	BUFFER_INDEX_COUNT	= 0;			// バッファーの配列の番号.
};

CLightingRender::CLightingRender()
	: m_pConstantBufferFrame	( nullptr )
{
}

CLightingRender::~CLightingRender()
{
}

//------------------------------------.
// 初期化.
//------------------------------------.
HRESULT CLightingRender::Init( ID3D11DeviceContext* pContext11 )
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
void CLightingRender::Render( const int& srvCount, const std::vector<ID3D11ShaderResourceView*>& srvList )
{
	CLightingRender::SetBuffer();

#if 1
	SLightState lightState = SLightState();
	lightState.Position		= D3DXVECTOR4( 5.0f, 5.0f, 0.0f, 0.0f );
	lightState.Vector		= D3DXVECTOR4( 0.0f, 1.0f, 0.0f, 0.0f );
	lightState.Color		= D3DXVECTOR4( 2.0f, 0.0f, 0.0f, 5.0f );
	lightState.ConeAngle	= D3DXVECTOR4( 0.0f, 0.0f, 0.0f, 0.0f );
	lightState.Type			= SLightState::POINT_LIGHT;

	CLightManager::PushLightStateQueue( lightState );

	static float rot = 0.0f;
	rot += 0.01f;

	float color_r = fabsf(cosf(rot));
	float color_g = fabsf(sinf(rot));
	float color_b = fabsf(cosf(rot)*sinf(rot));
	lightState.Position		= D3DXVECTOR4( -9.0f, 5.0f, 0.0f, 0.0f );
	lightState.Vector		= D3DXVECTOR4( 0.0f, 1.0f, 0.0f, 0.0f );
	lightState.Color		= D3DXVECTOR4( color_r, color_g, color_b, 10.0f );
	lightState.ConeAngle	= D3DXVECTOR4( 5.0f, 0.0f, 0.0f, 0.0f );
	lightState.Type			= SLightState::POINT_LIGHT;

	CLightManager::PushLightStateQueue( lightState );
#endif

	// シェーダーのコンスタントバッファに各種データを渡す.
	D3D11_MAPPED_SUBRESOURCE pData;
	if( SUCCEEDED( m_pContext11->Map( m_pConstantBufferFrame, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData ))){
		LIGHT_CBUFFER cb;

		cb.CameraPos = D3DXVECTOR4( CCameraManager::GetPosition(), 0.0f );

		// キュー情報を取得する.
		std::queue<SLightState> lightStateQueue = CLightManager::GetLightStateQueue();
		// キュー情報を受け取ったのでをクリアする.
		CLightManager::LightStateQueueClear();

		// ライトの情報をキューの数分設定.
		for( int i = 0; i < LIGHT_MAX_COUNT; i++ ){
			SLightState state = {};
			// キューが空じゃなければ.
			if( lightStateQueue.empty() == false ){
				// 値を取得し、取り出す.
				state = lightStateQueue.front();
				lightStateQueue.pop();
			}

			cb.LightPos[i]			= state.Position;	// 座標.
			cb.LightVec[i]			= state.Vector;		// 方向.
			cb.LightColor[i]		= state.Color;		// 色.
			cb.LightConeAngle[i]	= state.ConeAngle;	// コーン角度.
			cb.LightType[i].x		= static_cast<float>(state.Type);	// ライト種類.
		}

		// 余ったキューの後片付け.
		const int queueSize = static_cast<int>(lightStateQueue.size());
		for( int i = 0; i < queueSize; i++ ){
			lightStateQueue.pop();
		}

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

	m_pContext11->PSSetShaderResources( 0, srvCount, &srvList[0] );
	SetRasterizerState( ERS_STATE::Back );
	m_pContext11->Draw( 4, 0 );
	SetRasterizerState( ERS_STATE::None );
	std::vector<ID3D11ShaderResourceView*> resetSrvList(srvCount);
	m_pContext11->PSSetShaderResources( 0, srvCount, &resetSrvList[0] );
}

//------------------------------------.
// バッファの設定.
//------------------------------------.
void CLightingRender::SetBuffer()
{
	CRenderTexture::SetBuffer( BUFFER_COUNT_MAX );
}

//------------------------------------.
// テクスチャの初期化.
//------------------------------------.
HRESULT CLightingRender::InitBufferTex()
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