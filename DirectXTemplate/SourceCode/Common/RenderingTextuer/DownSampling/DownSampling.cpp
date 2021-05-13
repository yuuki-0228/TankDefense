#include "DownSampling.h"
#include "..\..\Shader\Shader.h"
#include "..\..\..\Object\CameraBase\CameraManager\CameraManager.h"
#include "..\..\..\Object\LightBase\LightManager\LightManager.h"
#include "..\..\..\Utility\ImGuiManager\ImGuiManager.h"
#include "..\..\..\Resource\GameParamResource\GameParamResource.h"

namespace
{
	const char*	SHADER_NAME			= "Data\\Shader\\RenderTexture\\Bloom\\DownSampling.hlsl";	// ピクセルシェーダー名.
	const char*	SHADER_ENTRY_NAME	= "PS_Main";		// ピクセルシェーダーエントリー名.
	const int	BUFFER_COUNT_MAX	= 1;	// バッファーの最大数.
};

CDownSamplingRender::CDownSamplingRender()
	: m_pConstantBufferFrame	( nullptr )
	, m_DownSamleParam			()
{
}

CDownSamplingRender::~CDownSamplingRender()
{
}

//------------------------------------.
// 初期化.
//------------------------------------.
HRESULT CDownSamplingRender::Init( ID3D11DeviceContext* pContext11 )
{
	if( FAILED( InitBase( pContext11 ) ))		return E_FAIL;
	if( FAILED( InitBufferTex() ))				return E_FAIL;
	if( FAILED( InitPixelShader( SHADER_NAME, SHADER_ENTRY_NAME ) )) return E_FAIL;
	if( FAILED( shader::CreateConstantBuffer( m_pDevice11, &m_pConstantBufferFrame, sizeof(C_BUFFER_PER_FRAME) ))) return E_FAIL;

	return S_OK;
}

//------------------------------------.
// 描画.
//------------------------------------.
void CDownSamplingRender::Render( const int& srvCount, const std::vector<ID3D11ShaderResourceView*>& srvList )
{
	CDownSamplingRender::SetBuffer();

	CImGuiManager::PushRenderProc( 
		[&]()
		{
			// ブルーム用のサンプリングの強さ.
			ImGui::Begin( "BloomRender" );
			ImGui::SliderFloat( "SOFT_THRSHOLD", &m_DownSamleParam.SoftThrshold, 0.0f, 1.0f );
			ImGui::SliderFloat( "THRSHOLD", &m_DownSamleParam.Thrshold, 0.0f, 1.0f );
			ImGui::SliderFloat( "Intensity", &m_DownSamleParam.Intensity, 0.01f, 10.0f );
			static CImGuiManager::SSuccess s_Success;
			if( ImGui::Button( "Save" ) ){
				s_Success.IsSucceeded = CGameParamResource::WritingParam( EParamNo::DownSamle, m_DownSamleParam, true );
			}
			if( ImGui::Button( "Load" ) ){
				m_DownSamleParam = CGameParamResource::GetParam<SDownSamplePrame>();
				s_Success.IsSucceeded = true;
			}
			s_Success.Render();
			ImGui::End();
		});
	// シェーダーのコンスタントバッファに各種データを渡す.
	D3D11_MAPPED_SUBRESOURCE pData;
	if( SUCCEEDED( m_pContext11->Map( m_pConstantBufferFrame, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData ))){
		C_BUFFER_PER_FRAME cb;

		const float knee = m_DownSamleParam.SoftThrshold * m_DownSamleParam.Thrshold;
		cb.SoftKneePram.x = m_DownSamleParam.Thrshold;
		cb.SoftKneePram.y = m_DownSamleParam.Thrshold+knee;
		cb.SoftKneePram.z = knee*2.0f;
		cb.SoftKneePram.w = 0.25f*knee*0.00001f;

		cb.Intensity.x = m_DownSamleParam.Intensity;

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
	m_pContext11->VSSetConstantBuffers( 1, 1, &m_pConstantBufferFrame );// 頂点シェーダ.
	m_pContext11->PSSetConstantBuffers( 1, 1, &m_pConstantBufferFrame );// ピクセルシェーダー.

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
void CDownSamplingRender::SetBuffer()
{
	CRenderTexture::SetBuffer( BUFFER_COUNT_MAX );
}

//------------------------------------.
// テクスチャの初期化.
//------------------------------------.
HRESULT CDownSamplingRender::InitBufferTex()
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

	for( int i = 0; i < BUFFER_COUNT_MAX; i++ ){
		if( FAILED( CreateBufferTex(
			texDesc,
			&m_pRenderTargetViewList[i],
			&m_pShaderResourceViewList[i],
			&m_pTexture2DList[i] ))) return E_FAIL;
	}

	return S_OK;
}