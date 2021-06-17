#include "SpriteRender.h"
#include "Sprite.h"
#include "..\Shader\Shader.h"

namespace
{
	const char* SHADER_NAME = "Data\\Shader\\Sprite\\Sprite.hlsl";
};

CSpriteRender::CSpriteRender()
	: m_pSpriteQueue	()
	, m_pVertexShader	( nullptr )
	, m_pPixelShader	( nullptr )
	, m_pVertexLayout	( nullptr )
	, m_pConstantBuffer	( nullptr )
	, m_pSampleLinears	()
{
}

CSpriteRender::~CSpriteRender()
{
	Release();
}

//---------------------------------------.
// 初期化.
//---------------------------------------.
HRESULT CSpriteRender::Init( ID3D11DeviceContext* pContext11 )
{
	// デバイスの取得.
	if( FAILED( SetDevice(pContext11) ))	return E_FAIL;
	if( FAILED( InitShader() ))				return E_FAIL;
	if( FAILED( InitSample() ))				return E_FAIL;
	if( FAILED( InitConstantBuffer() ))		return E_FAIL;

	CLog::Print( "画像描画クラス作成 : 成功" );

	return S_OK;
}

//---------------------------------------.
// 解放.
//---------------------------------------.
void CSpriteRender::Release()
{
	const int queueSize = static_cast<int>(m_pSpriteQueue.size());
	for( int i = 0; i < queueSize; i++ ){
		m_pSpriteQueue.front().first = nullptr;
		m_pSpriteQueue.pop();
	}

	SAFE_RELEASE( m_pConstantBuffer );
	for( auto& s : m_pSampleLinears ) SAFE_RELEASE( s );
	SAFE_RELEASE( m_pPixelShader );
	SAFE_RELEASE( m_pVertexLayout );
	SAFE_RELEASE( m_pVertexShader );
	
	m_pDevice11 = nullptr;
	m_pContext11 = nullptr;
}

//---------------------------------------.
// 描画.
//---------------------------------------.
void CSpriteRender::Render()
{
	const int queueSize = static_cast<int>(m_pSpriteQueue.size());
	for( int i = 0; i < queueSize; i++ ){
		const CSprite*				pSprite		= m_pSpriteQueue.front().first;
		const SSpriteRenderState	renderState	= m_pSpriteQueue.front().second;
		if( pSprite == nullptr ) continue;

		// 画像情報の取得.
		ID3D11Buffer*				pVertexBuffer	= pSprite->GetVertexBufferUI();
		ID3D11ShaderResourceView*	pTexture		= pSprite->GetTexture();
	
		Render( renderState, pTexture, pVertexBuffer );

		m_pSpriteQueue.pop();
	}
}

//---------------------------------------.
// 描画.
//---------------------------------------.
void CSpriteRender::Render( 
	const SSpriteRenderState& renderState,
	ID3D11ShaderResourceView* pTexture,
	ID3D11Buffer* pVertexBufferUI )
{
	const SSpriteAnimState animState	= renderState.AnimState;
	//　ワールド行列.
	D3DXMATRIX mWorld = renderState.Tranceform.GetWorldMatrix();

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

		cb.mWVP	= mWorld;
		cb.mW	= mWorld;
		D3DXMatrixTranspose( &cb.mWVP, &cb.mWVP );
		D3DXMatrixTranspose( &cb.mW, &cb.mW );

		// ビューポートの幅,高さを渡す.
		cb.vViewPort.x	= static_cast<float>(WND_W);
		cb.vViewPort.y	= static_cast<float>(WND_H);

		// 色を渡す.
		cb.vColor = renderState.Color;

		// テクスチャ座標.
		cb.vUV = animState.UV;

		memcpy_s( pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));

		m_pContext11->Unmap( m_pConstantBuffer, 0 );
	}

	// シェーダーの設定.
	m_pContext11->VSSetShader( m_pVertexShader, nullptr, 0 );
	m_pContext11->PSSetShader( m_pPixelShader, nullptr, 0 );

	// コンスタントバッファの設定.
	m_pContext11->VSSetConstantBuffers( 0, 1, &m_pConstantBuffer );
	m_pContext11->PSSetConstantBuffers( 0, 1, &m_pConstantBuffer );

	// 頂点情報の設定.
	UINT stride = sizeof(SpriteVertex);
	UINT offset = 0;
	m_pContext11->IASetVertexBuffers( 0, 1, &pVertexBufferUI, &stride, &offset );
	m_pContext11->IASetInputLayout( m_pVertexLayout );

	// 画像情報の設定.
	m_pContext11->PSSetSamplers( 0, 1, &m_pSampleLinears[renderState.SmaplerNo] );
	m_pContext11->PSSetShaderResources( 0, 1, &pTexture );
	m_pContext11->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

	// 描画.
	SetBlend( true );
	SetDeprh( false );
	SetRasterizerState( ERS_STATE::Back );
	m_pContext11->Draw( 4, 0 );
	SetRasterizerState( ERS_STATE::None );
	SetDeprh( true );
	SetBlend( false );
}

//---------------------------------------.
// 画像データの追加.
//---------------------------------------.
void CSpriteRender::PushSpriteData( const spriteRenderPair& renderPair )
{
	m_pSpriteQueue.push( renderPair );
}
void CSpriteRender::PushSpriteData( CSprite* pSprite, const SSpriteRenderState& renderState )
{
	m_pSpriteQueue.push( { pSprite, renderState } );
}
void CSpriteRender::PushSpriteData( CSprite* pSprite )
{
	SSpriteRenderState state;
	state.Tranceform.Position = pSprite->GetRenderPos();
	state.AnimState = pSprite->GetAnimState();
	PushSpriteData( pSprite, state );
}

//---------------------------------------.
// シェーダーの作成.
//---------------------------------------.
HRESULT CSpriteRender::InitShader()
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
			SHADER_NAME,		// シェーダーファイル名.
			"VS_MainUI",			// シェーダーエントリーポイント.
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
	if (FAILED(
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


	// HLSLからピクセルシェーダーのブロブを作成.
	if( FAILED(
		shader::InitShader(
			SHADER_NAME,		// シェーダーファイル名.
			"PS_Main",			// シェーダーエントリーポイント.
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

//---------------------------------------.
// 定数バッファの作成.
//---------------------------------------.
HRESULT CSpriteRender::InitConstantBuffer()
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
HRESULT CSpriteRender::InitSample()
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