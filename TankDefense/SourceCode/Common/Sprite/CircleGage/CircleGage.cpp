#include "CircleGage.h"
#include "..\..\Shader\Shader.h"
#include "..\..\..\Object\CameraBase\CameraManager\CameraManager.h"

#include <algorithm>

namespace
{
	const char* SHADER_NAME = "Data\\Shader\\Sprite\\GageSprite.hlsl";
	const char* PS_SHADER_ENTRY_NAMES[] =
	{
		"PS_CircleMain",
	};
};

CCircleGageSprite::CCircleGageSprite()
	: m_CenterPosition	( 0.0f, 0.0f )
	, m_StartVector		( 0.0f, 1.0f )
	, m_Value			( 0.5f )
	, m_Angle			( static_cast<float>(D3DX_PI)*2.0f )
	, m_IsClockwise		( true )
{
}

CCircleGageSprite::~CCircleGageSprite()
{
	CSprite::Release();
}

//---------------------------------------.
// 初期化.
//---------------------------------------.
HRESULT CCircleGageSprite::Init( ID3D11DeviceContext* pContext11, const char* texturePath )
{
	if( FAILED( SetDevice( CDirectX11::GetContext() ) ))	return E_FAIL;
	if( FAILED( VertexInitShader( SHADER_NAME ) ))			return E_FAIL;
	if( FAILED( CCircleGageSprite::PixelInitShader() ))		return E_FAIL;
	if( FAILED( InitSample() ))								return E_FAIL;
	if( FAILED( CCircleGageSprite::InitConstantBuffer() ))	return E_FAIL;

	return S_OK;
}

//---------------------------------------.
// 解放.
//---------------------------------------.
void CCircleGageSprite::Release()
{
	CSprite::Release();
}

//---------------------------------------.
// 描画.
//---------------------------------------.
void CCircleGageSprite::Render( SSpriteRenderState* pRenderState )
{
	if( m_pSrcTexture == nullptr ) return;
	if( m_pVertexShaderUI == nullptr ) return;

	SSpriteRenderState* renderState = pRenderState == nullptr ? &m_SpriteRenderState : pRenderState;
	SSpriteAnimState* animState = &renderState->AnimState;

	animState->AnimUpdate();		// アニメーションの更新.
	animState->UVScrollUpdate();	// スクロールの更新.

	//　ワールド行列.
	D3DXMATRIX mWorld = renderState->Tranceform.GetWorldMatrix();

	//　ワールド行列, スケール行列, 回転行列, 平行移動行列.
	D3DXMATRIX mWVP;

	// シェーダーのコンスタントバッファに各種データを渡す.
	D3D11_MAPPED_SUBRESOURCE pData;
	CBUFFER_PER_FRAME cb;	// コンスタントバッファ.

	// バッファ内のデータの書き換え開始時にMap.
	if( SUCCEEDED( m_pContext11->Map(
		m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData ))){

		cb.mWVP	= mWorld * CCameraManager::GetViewProjMatrix();
		cb.mW	= mWorld;
		D3DXMatrixTranspose( &cb.mWVP, &cb.mWVP );
		D3DXMatrixTranspose( &cb.mW, &cb.mW );

		// ビューポートの幅,高さを渡す.
		cb.vViewPort.x	= static_cast<float>(WND_W);
		cb.vViewPort.y	= static_cast<float>(WND_H);

		// 色を渡す.
		cb.vColor = renderState->Color;
		// テクスチャ座標.
		cb.vUV = animState->UV;

		// 円の中心座標.
		// 0~1までなので、0.5が中心.
		cb.vCenterPos = m_CenterPosition;

		// 円の開始ベクトル.
		// 2Dなので、上向きがマイナス.
		cb.vStartVector = m_StartVector;

		// 値 0~1 をradianとして渡す.
		cb.Value	= m_Angle*std::clamp(m_Value, 0.0f, 1.0f);

		// 時計回りにするかどうか設定.
		cb.IsClockwise = m_IsClockwise == true ? 1.0f : 0.0f;

		memcpy_s( pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb) );

		m_pContext11->Unmap( m_pConstantBuffer, 0 );
	}

	// 使用するシェーダのセット.
	m_pContext11->VSSetShader( m_pVertexShaderUI, nullptr, 0 );	// 頂点シェーダ.
	m_pContext11->PSSetShader( m_pPixelShaders[EGageNo_Circle], nullptr, 0 );	// ピクセルシェーダ.

	// このコンスタントバッファをどのシェーダで使用するか？.
	m_pContext11->VSSetConstantBuffers( 0, 1, &m_pConstantBuffer );	// 頂点シェーダ.
	m_pContext11->PSSetConstantBuffers( 0, 1, &m_pConstantBuffer );	// ピクセルシェーダー.

	// 頂点バッファをセット.
	UINT stride = sizeof(SpriteVertex); // データの間隔.
	UINT offset = 0;
	m_pContext11->IASetVertexBuffers( 0, 1, &m_pVertexBufferUI, &stride, &offset );

	// 頂点インプットレイアウトをセット.
	m_pContext11->IASetInputLayout( m_pVertexLayout );

	// プリミティブ・トポロジーをセット.
	m_pContext11->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

	// テクスチャをシェーダーに渡す.
	m_pContext11->PSSetSamplers( 0, 1, &m_pSampleLinears[renderState->SmaplerNo] );
	m_pContext11->PSSetShaderResources( 0, 1, &m_pSrcTexture );
	m_pContext11->PSSetShaderResources( 1, 1, &m_pDestTexture );
	m_pContext11->PSSetShaderResources( 2, 1, &m_pMaskTexture );
	m_pContext11->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

	// レンダリング.
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
void CCircleGageSprite::Render3D( const bool& isBillboard, SSpriteRenderState* pRenderState )
{
	if( m_pSrcTexture == nullptr ) return;
	if( m_pVertexBuffer3D == nullptr ) return;

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

	//　ワールド行列, スケール行列, 回転行列, 平行移動行列.
	D3DXMATRIX mWVP;

	// シェーダーのコンスタントバッファに各種データを渡す.
	D3D11_MAPPED_SUBRESOURCE pData;
	CBUFFER_PER_FRAME cb;	// コンスタントバッファ.

	// バッファ内のデータの書き換え開始時にMap.
	if( SUCCEEDED( m_pContext11->Map(
		m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData ))){

		cb.mWVP	= mWorld * CCameraManager::GetViewProjMatrix();
		cb.mW	= mWorld;
		D3DXMatrixTranspose( &cb.mWVP, &cb.mWVP );
		D3DXMatrixTranspose( &cb.mW, &cb.mW );

		// ビューポートの幅,高さを渡す.
		cb.vViewPort.x	= static_cast<float>(WND_W);
		cb.vViewPort.y	= static_cast<float>(WND_H);

		// 色を渡す.
		cb.vColor = renderState->Color;
		// テクスチャ座標.
		cb.vUV = animState->UV;

		// 円の中心座標.
		// 0~1までなので、0.5が中心.
		cb.vCenterPos = m_CenterPosition;

		// 円の開始ベクトル.
		// 2Dなので、上向きがマイナス.
		cb.vStartVector = m_StartVector;

		// 値 0~1 をradianとして渡す.
		cb.Value	= m_Angle*std::clamp(m_Value, 0.0f, 1.0f);

		// 時計回りにするかどうか設定.
		cb.IsClockwise = m_IsClockwise == true ? 1.0f : 0.0f;

		memcpy_s( pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb) );

		m_pContext11->Unmap( m_pConstantBuffer, 0 );
	}

	// 使用するシェーダのセット.
	m_pContext11->VSSetShader( m_pVertexShader3D, nullptr, 0 );	// 頂点シェーダ.
	m_pContext11->PSSetShader( m_pPixelShaders[EGageNo_Circle], nullptr, 0 );	// ピクセルシェーダ.

	// このコンスタントバッファをどのシェーダで使用するか？.
	m_pContext11->VSSetConstantBuffers( 0, 1, &m_pConstantBuffer );	// 頂点シェーダ.
	m_pContext11->PSSetConstantBuffers( 0, 1, &m_pConstantBuffer );	// ピクセルシェーダー.

	// 頂点バッファをセット.
	UINT stride = sizeof(SpriteVertex); // データの間隔.
	UINT offset = 0;
	m_pContext11->IASetVertexBuffers( 0, 1, &m_pVertexBuffer3D, &stride, &offset );

	// 頂点インプットレイアウトをセット.
	m_pContext11->IASetInputLayout( m_pVertexLayout );

	// プリミティブ・トポロジーをセット.
	m_pContext11->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

	// テクスチャをシェーダーに渡す.
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
// ピクセルシェーダーの作成.
//---------------------------------------.
HRESULT CCircleGageSprite::PixelInitShader()
{
	ID3DBlob* pCompiledShader = nullptr;
	ID3DBlob* pErrors = nullptr;
	UINT uCompileFlag = 0;
#ifdef _DEBUG
	uCompileFlag =
		D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
#endif	// #ifdef _DEBUG
	int i = 0;
	m_pPixelShaders.resize(EGageNo_Max);
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
HRESULT CCircleGageSprite::InitConstantBuffer()
{
	if( FAILED(
		shader::CreateConstantBuffer( m_pDevice11, &m_pConstantBuffer, sizeof(CBUFFER_PER_FRAME) ))) {
		ERROR_MESSAGE("定数バッファ作成 : 失敗");
		return E_FAIL;
	}
	return S_OK;
}
