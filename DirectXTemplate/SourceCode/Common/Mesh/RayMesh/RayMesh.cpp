#include "RayMesh.h"
#include "..\..\Shader\Shader.h"
#include "..\..\..\Object\CameraBase\CameraManager\CameraManager.h"
#include "..\..\..\Utility\Math\Math.h"

namespace
{
	const char* SHADER_NAME = "Data\\Shader\\RayMesh.hlsl";
};

CRayMesh::CRayMesh()
	: m_pVertexShader	( nullptr )
	, m_pPixelShader	( nullptr )
	, m_pVertexLayout	( nullptr )
	, m_pConstantBuffer	( nullptr )
	, m_pVertexBuffer	( nullptr )
	, m_Vector			( 0.0f, 0.0f, 0.0f )
{
}

CRayMesh::~CRayMesh()
{
	Release();
}

// 初期化関数.
HRESULT CRayMesh::Init( ID3D11DeviceContext* pContext11 )
{
	if( FAILED( SetDevice( pContext11 ) ))	return E_FAIL;
	if( FAILED( InitShader() ))				return E_FAIL;
	if( FAILED( InitVertexBuffer() ))		return E_FAIL;
	if( FAILED( shader::CreateConstantBuffer( m_pDevice11, &m_pConstantBuffer, sizeof(C_BUFFER_PER_FRAME) ))) return E_FAIL;

	return S_OK;
}

// 解放.
void CRayMesh::Release()
{
	SAFE_RELEASE( m_pVertexShader );
	SAFE_RELEASE( m_pPixelShader );
	SAFE_RELEASE( m_pVertexLayout );
	SAFE_RELEASE( m_pConstantBuffer );
	SAFE_RELEASE( m_pVertexBuffer );

	
	m_pDevice11 = nullptr;
	m_pContext11 = nullptr;
}

// 描画関数.
void CRayMesh::Render()
{
	D3DXMATRIX mWorld, mScale, mRot, mTran;

	// 拡大縮小行列作成.
	D3DXMatrixScaling( &mScale, m_Tranceform.Scale.x, m_Tranceform.Scale.y, m_Tranceform.Scale.z );
	// 平行移動行列.
	D3DXMatrixTranslation( &mTran, m_Tranceform.Position.x, m_Tranceform.Position.y, m_Tranceform.Position.z );
	//回転行列.
	D3DXQUATERNION q = Math::GetRotationBetween( Math::Z_AXIS, m_Vector );
	D3DXMatrixRotationQuaternion( &mRot, &q );

	// ワールド行列作成.
	mWorld = mScale * mRot * mTran;

	// シェーダーのコンスタントバッファに各種データを渡す.
	D3D11_MAPPED_SUBRESOURCE pData;
	C_BUFFER_PER_FRAME cb;	// コンスタントバッファ.

	// バッファ内のデータの書き換え開始時にMap.
	if( SUCCEEDED( m_pContext11->Map(
		m_pConstantBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&pData ))){

		cb.mWVP	= mWorld * CCameraManager::GetViewProjMatrix();
		D3DXMatrixTranspose( &cb.mWVP, &cb.mWVP );

		cb.vColor = m_Color;

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
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	m_pContext11->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, &stride, &offset );
	m_pContext11->IASetInputLayout( m_pVertexLayout );

	m_pContext11->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_LINELIST );

	// 描画.
	m_pContext11->Draw( 2, 0 );
}

// シェーダーの作成.
HRESULT CRayMesh::InitShader()
{
	ID3DBlob* pCompiledShader = nullptr;
	ID3DBlob* pErrors = nullptr;
	UINT uCompileFlag = 0;
#ifdef _DEBUG
	uCompileFlag =
		D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
#endif	// #ifdef _DEBUG

	//===========================.
	// 頂点シェーダーの作成.
	//===========================.
	// HLSLからバーテックスシェーダーのブロブを作成.
	if( FAILED(
		shader::InitShader(
			SHADER_NAME,		// シェーダーファイル名.
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
		&m_pVertexShader ))) {
		ERROR_MESSAGE("頂点シェーダー作成 : 失敗");
		return E_FAIL;
	}

	// 頂点インプットレイアウトを定義.
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		shader::GetPositionInputElement(),
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


	//===========================.
	// ピクセルシェーダーの作成.
	//===========================.
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

// 頂点バッファの作成.
HRESULT CRayMesh::InitVertexBuffer()
{
	// レイのバーテックスバッファ作成.
	CD3D11_BUFFER_DESC bd;
	bd.Usage			= D3D11_USAGE_DEFAULT;
	bd.ByteWidth		= sizeof(VERTEX)*2;// 二点分.
	bd.BindFlags		= D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags	= 0;
	bd.MiscFlags		= 0;

	// とりあえずzベクトルのレイを作成.
	D3DXVECTOR3 pos[2] =
	{
		{ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }
	};

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = pos;	// レイの座標をセット.
	if( FAILED( m_pDevice11->CreateBuffer( &bd, &InitData, &m_pVertexBuffer ))){	
		ERROR_MESSAGE( "バーテックスバッファ作成失敗" );
		return E_FAIL;
	}

	return S_OK;
}