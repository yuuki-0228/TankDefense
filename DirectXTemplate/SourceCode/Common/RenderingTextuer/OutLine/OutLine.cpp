#include "OutLine.h"
#include "..\..\Shader\Shader.h"
#include "..\..\..\Utility\ImGuiManager\ImGuiManager.h"
#include "..\..\..\Utility\FileManager\FileManager.h"
#include "..\..\..\Resource\GameParamResource\GameParamResource.h"

namespace
{
	const char*	SHADER_NAME			= "Data\\Shader\\RenderTexture\\OutLine\\OutLine_PS.hlsl";	// ピクセルシェーダー名.
	const char*	SHADER_ENTRY_NAME	= "PS_Main";	// ピクセルシェーダーエントリー名.
};

COutLineRender::COutLineRender()
	: m_pConstantBufferFrame	( nullptr )
	, m_OutlineBuffer			()
{
}

COutLineRender::~COutLineRender()
{
}

//------------------------------------.
// 初期化.
//------------------------------------.
HRESULT COutLineRender::Init( ID3D11DeviceContext* pContext11 )
{
	if( FAILED( InitBase( pContext11 ) ))		return E_FAIL;
	if( FAILED( InitBufferTex() ))				return E_FAIL;
	if( FAILED( InitPixelShader( SHADER_NAME, SHADER_ENTRY_NAME ) )) return E_FAIL;
	if( FAILED( shader::CreateConstantBuffer( m_pDevice11, &m_pConstantBufferFrame, sizeof(OUTLINE_CBUFFER) ))) return E_FAIL;
	m_OutlineBuffer = CGameParamResource::GetParam<OUTLINE_CBUFFER>();

	return S_OK;
}

//------------------------------------.
// 描画.
//------------------------------------.
void COutLineRender::Render( const int& srvCount, const std::vector<ID3D11ShaderResourceView*>& srvList )
{
	COutLineRender::SetBuffer();

	CImGuiManager::PushRenderProc( 
		[&]()
		{
			ImGui::Begin( "OutLineRender" );
			ImGui::SliderFloat( "NormalPower",		&m_OutlineBuffer.Sample.NormalPower,	0.0f, 4.0f );
			ImGui::SliderFloat( "NormalCompare",	&m_OutlineBuffer.Sample.NormalCompare,	0.0f, 1.0f );
			ImGui::SliderFloat( "DepthPower",		&m_OutlineBuffer.Sample.DepthPower,		0.0f, 4.0f );
			ImGui::SliderFloat( "DepthCompare",		&m_OutlineBuffer.Sample.DepthCompare,	0.0f, 1.0f );
			ImGui::ColorPicker4("Color",			(float*)&m_OutlineBuffer.Color );
			static CImGuiManager::SSuccess s_Success;
			if( ImGui::Button( "Save" ) ){
				s_Success.IsSucceeded = CGameParamResource::WritingParam( EParamNo::OutLine, m_OutlineBuffer, true );
			}
			if( ImGui::Button( "Load" ) ){
				m_OutlineBuffer = CGameParamResource::GetParam<OUTLINE_CBUFFER>();
				s_Success.IsSucceeded = true;
			}
			s_Success.Render();
			ImGui::End();
		});

	// シェーダーのコンスタントバッファに各種データを渡す.
	D3D11_MAPPED_SUBRESOURCE pData;
	if( SUCCEEDED( m_pContext11->Map( m_pConstantBufferFrame, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData ))){
		// メモリ領域をコピー.
		memcpy_s( pData.pData, pData.RowPitch, (void*)(&m_OutlineBuffer), sizeof(m_OutlineBuffer) );
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
void COutLineRender::SetBuffer()
{
	CRenderTexture::SetBuffer( EOutLineNo_MAX );
}

//------------------------------------.
// テクスチャの初期化.
//------------------------------------.
HRESULT COutLineRender::InitBufferTex()
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

	m_pRenderTargetViewList.resize( EOutLineNo_MAX );
	m_pShaderResourceViewList.resize( EOutLineNo_MAX );
	m_pTexture2DList.resize( EOutLineNo_MAX );

	for( int i = 0; i < EOutLineNo_MAX; i++ ){
		if( FAILED( CreateBufferTex(
			texDesc,
			&m_pRenderTargetViewList[i],
			&m_pShaderResourceViewList[i],
			&m_pTexture2DList[i] ))) return E_FAIL;
	}

	return S_OK;
}