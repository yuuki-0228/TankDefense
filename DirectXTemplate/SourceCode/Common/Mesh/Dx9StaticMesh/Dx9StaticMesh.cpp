#include "Dx9StaticMesh.h"
#include "..\..\..\Object\CameraBase\CameraManager\CameraManager.h"
#include "..\..\..\Object\LightBase\LightManager\LightManager.h"
#include "..\..\Shader\Shader.h"

// シェーダーファイル名.
const char SHADER_VS_NAME[] = "Data\\Shader\\Mesh.hlsl";
const char SHADER_PS_NAME[] = "Data\\Shader\\MeshPS.hlsl";

CDX9StaticMesh::CDX9StaticMesh()
	: m_hWnd				( nullptr )
	, m_pDevice9			( nullptr )
	, m_pVertexShader		( nullptr )
	, m_pVertexLayout		( nullptr )
	, m_pPixelShader		( nullptr )
	, m_pCBufferPerMesh		( nullptr )
	, m_pCBufferPerMaterial	( nullptr )
	, m_pCBufferPerFrame	( nullptr )
	, m_pVertexBuffer		( nullptr )
	, m_ppIndexBuffer		( nullptr )
	, m_pSampleLinear		( nullptr )
	, m_pMesh				( nullptr )
	, m_NumMaterials		( 0 )
	, m_pMaterials			( nullptr )
	, m_NumAttr				( 0 )
	, m_AttrID				()
{
}

CDX9StaticMesh::~CDX9StaticMesh()
{
	Release();
}

// 初期化関数.
HRESULT CDX9StaticMesh::Init(
	HWND hWnd, 
	ID3D11Device* pDevice11,
	ID3D11DeviceContext* pContext11, 
	LPDIRECT3DDEVICE9 pDevice9,
	const char* fileName )
{
	m_hWnd = hWnd;
	m_pDevice9 = pDevice9;
	if( FAILED( SetDevice( pContext11 ) )) return E_FAIL;
	if( FAILED( LoadXMesh(fileName) )) return E_FAIL; 
	if( FAILED( InitShader() )) return E_FAIL;

	std::string msg = fileName;
	msg += " 読み込み : 成功";
	CLog::Print( std::string( msg.c_str() ).c_str() );

	return S_OK;
}

// メッシュ読み込み.
HRESULT CDX9StaticMesh::LoadXMesh(const char* fileName)
{
	// マテリアルバッファ.
	LPD3DXBUFFER pD3DXMtrlBuffer = nullptr;

	// Xファイルのロード.
	if( FAILED( D3DXLoadMeshFromX(
		fileName,				// ファイル名.
		D3DXMESH_SYSTEMMEM		// システムメモリに読み込み.
			| D3DXMESH_32BIT,	// 32bit.
		m_pDevice9, 
		nullptr,
		&pD3DXMtrlBuffer,		// (out)マテルアル情報.
		nullptr,
		&m_NumMaterials,		// (out)マテリアル数.
		&m_pMesh ))){			// (out)メッシュオブジェクト.
		ERROR_MESSAGE("Xファイル読込 : 失敗");
		return E_FAIL;
	}

	D3D11_BUFFER_DESC		bd;			// Dx11バッファ構造体.
	D3D11_SUBRESOURCE_DATA	InitData;	// 初期化データ.

	// 読み込んだ情報から必要な情報を抜き出す.
	D3DXMATERIAL* d3dxMaterials = static_cast<D3DXMATERIAL*>(pD3DXMtrlBuffer->GetBufferPointer());
	// マテリアル数分の領域を確保.
	m_pMaterials	= new MY_MATERIAL[m_NumMaterials]();
	m_ppIndexBuffer	= new ID3D11Buffer*[m_NumMaterials]();
	// マテリアル数分繰り返し.
	for( DWORD No = 0; No < m_NumMaterials; No++ ){
		// インデックスバッファの初期化.
		m_ppIndexBuffer[No] = nullptr;

		// マテリアル情報のコピー.
		// アンビエント.
		m_pMaterials[No].Ambient.x = d3dxMaterials[No].MatD3D.Ambient.r;
		m_pMaterials[No].Ambient.y = d3dxMaterials[No].MatD3D.Ambient.g;
		m_pMaterials[No].Ambient.z = d3dxMaterials[No].MatD3D.Ambient.b;
		m_pMaterials[No].Ambient.w = d3dxMaterials[No].MatD3D.Ambient.a;
		// ディフューズ.
		m_pMaterials[No].Diffuse.x = d3dxMaterials[No].MatD3D.Diffuse.r;
		m_pMaterials[No].Diffuse.y = d3dxMaterials[No].MatD3D.Diffuse.g;
		m_pMaterials[No].Diffuse.z = d3dxMaterials[No].MatD3D.Diffuse.b;
		m_pMaterials[No].Diffuse.w = d3dxMaterials[No].MatD3D.Diffuse.a;
		// スペキュラ.
		m_pMaterials[No].Specular.x = d3dxMaterials[No].MatD3D.Specular.r;
		m_pMaterials[No].Specular.y = d3dxMaterials[No].MatD3D.Specular.g;
		m_pMaterials[No].Specular.z = d3dxMaterials[No].MatD3D.Specular.b;
		m_pMaterials[No].Specular.w = d3dxMaterials[No].MatD3D.Specular.a;

		// (その面に)テクスチャが貼られているか？.
		if( d3dxMaterials[No].pTextureFilename != nullptr &&
			lstrlen(d3dxMaterials[No].pTextureFilename) > 0 ){

			char path[128] = "";
			int path_count = lstrlen(fileName);
			for( int k = path_count; k >= 0; k-- ){
				if (fileName[k] == '\\' ){
					for( int j = 0; j <= k; j++ ) path[j] = fileName[j];
					path[k + 1] = '\0';
					break;
				}
			}
			// パスとテクスチャファイル名を連結.
			strcat_s( path, sizeof(path), d3dxMaterials[No].pTextureFilename );

			// テクスチャファイル名をコピー.
			strcpy_s( m_pMaterials[No].szTextureName, sizeof(m_pMaterials[No].szTextureName), path );

			// テクスチャ作成.
			if( FAILED( D3DX11CreateShaderResourceViewFromFile(
				m_pDevice11, 
				m_pMaterials[No].szTextureName,	// テクスチャファイル名.
				nullptr, 
				nullptr,
				&m_pMaterials[No].pTexture,		// (out)テクスチャオブジェクト.
				nullptr ))){
				std::string text = m_pMaterials[No].szTextureName;
				ERROR_MESSAGE( text + " テクスチャ読込 : 失敗" );
				return E_FAIL;
			}

			// 法線テクスチャを取得.
			std::string normTexName = m_pMaterials[No].szTextureName;
			size_t i = normTexName.find(".");
			normTexName.insert( i , "_norm" );
			if( FAILED( D3DX11CreateShaderResourceViewFromFile(
				m_pDevice11, 
				normTexName.c_str(),	// テクスチャファイル名.
				nullptr, 
				nullptr,
				&m_pMaterials[No].pNormTexture,		// (out)テクスチャオブジェクト.
				nullptr ))){
				m_pMaterials[No].pNormTexture = nullptr;
			}
		} else {
			std::string text = fileName;
			// テクスチャ作成.
			if( FAILED( D3DX11CreateShaderResourceViewFromFile(
				m_pDevice11, 
				"Data\\Mesh\\noTex.png",	// テクスチャファイル名.
				nullptr, 
				nullptr,
				&m_pMaterials[No].pTexture,	// (out)テクスチャオブジェクト.
				nullptr ))){
				ERROR_MESSAGE( text + " テクスチャ読込 : 失敗" );
				return E_FAIL;
			}
			text += "にテクスチャがないため別のテクスチャを差し込みます。";
			CLog::Print( text.c_str() );
		}
	}

	//------------------------------------------------
	//	インデックスバッファ作成.
	//------------------------------------------------
	// メッシュの属性情報を得る.
	// 属性情報でインデックスバッファから細かいマテリアルごとのインデックスバッファを分離できる.
	D3DXATTRIBUTERANGE* pAttrTable = nullptr;

	// メッシュの面および頂点の順番変更を制御し、パフォーマンスを最適化する.
	// D3DXMESHOPT_COMPACT : 面の順番を変更し、使用されていない頂点と面を削除する.
	// D3DXMESHOPT_ATTRSORT : パフォーマンスを上げる為、面の順番を変更して最適化を行う.
	m_pMesh->OptimizeInplace( D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT, nullptr, nullptr, nullptr, nullptr);
	// 属性テーブルの取得.
	m_pMesh->GetAttributeTable( nullptr, &m_NumAttr );
	pAttrTable = new D3DXATTRIBUTERANGE[m_NumAttr];
	if( FAILED( m_pMesh->GetAttributeTable( pAttrTable, &m_NumAttr ) )){
		ERROR_MESSAGE( "属性ﾃｰﾌﾞﾙ取得失敗" );
		return E_FAIL;
	}

	// 同じくLockしないと取り出せない.
	int* pIndex = nullptr;
	m_pMesh->LockIndexBuffer( D3DLOCK_READONLY, (void**)&pIndex );
	// 属性ごとのインデックスバッファを作成.
	for( DWORD No = 0; No < m_NumAttr; No++ ){
		m_AttrID[No] = pAttrTable[No].AttribId;
		// Dx9のインデックスバッファからの情報で、Dx11のインデックスバッファを作成.
		bd.Usage			= D3D11_USAGE_DEFAULT;
		bd.ByteWidth		= sizeof(int)*pAttrTable[No].FaceCount * 3;//面数×3で頂点数.
		bd.BindFlags		= D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags	= 0;
		bd.MiscFlags		= 0;
		// 大きいインデックスバッファ内のオフセット(×3する).
		InitData.pSysMem = &pIndex[pAttrTable[No].FaceStart*3];

		if( FAILED( m_pDevice11->CreateBuffer( &bd, &InitData, &m_ppIndexBuffer[No] ) )){
			ERROR_MESSAGE( "インデックスバッファ作成 : 失敗" );
			return E_FAIL;
		}
		// 面の数をコピー.
		m_pMaterials[m_AttrID[No]].dwNumFace = pAttrTable[No].FaceCount;
	}
	// 属性テーブルの削除.
	delete[] pAttrTable;
	// 使用済みのインデックスの解放.
	m_pMesh->UnlockIndexBuffer();
	// 不要になったマテリアルバッファを解放.
	SAFE_RELEASE( pD3DXMtrlBuffer );

	//------------------------------------------------
	//	頂点バッファの作成.
	//------------------------------------------------
	// Dx9の場合、mapではなくLockで頂点バッファからデータを取り出す.
	LPDIRECT3DVERTEXBUFFER9 pVB = nullptr;
	m_pMesh->GetVertexBuffer(&pVB);
	DWORD dwStride = m_pMesh->GetNumBytesPerVertex();
	BYTE* pVertices = nullptr;
	VERTEX* pVertex = nullptr;
	if( SUCCEEDED( pVB->Lock(0, 0, (VOID**)&pVertices, 0) )){
		pVertex = (VERTEX*)pVertices;
		// Dx9の頂点バッファからの情報で、Dx11頂点バッファを作成.
		bd.Usage			= D3D11_USAGE_DEFAULT;
		bd.ByteWidth		= m_pMesh->GetNumBytesPerVertex()*m_pMesh->GetNumVertices();
		bd.BindFlags		= D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags	= 0;
		bd.MiscFlags		= 0;
		InitData.pSysMem	= pVertex;
		if( FAILED( m_pDevice11->CreateBuffer( &bd, &InitData, &m_pVertexBuffer ) )){
			ERROR_MESSAGE( "頂点バッファ作成 : 失敗" );
			return E_FAIL;
		}
		pVB->Unlock();
	}
	SAFE_RELEASE(pVB);	// 頂点バッファ解放.


	// テクスチャ用のサンプラ構造体.
	D3D11_SAMPLER_DESC samDesc;
	ZeroMemory( &samDesc, sizeof(samDesc) );
	samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;//ﾘﾆｱﾌｨﾙﾀ(線形補間).
	samDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	// サンプラ作成.
	if( FAILED( m_pDevice11->CreateSamplerState( &samDesc, &m_pSampleLinear ))){
		ERROR_MESSAGE( "サンプラ作成 : 失敗" );
		return E_FAIL;
	}


	return S_OK;
}

// 解放関数.
void CDX9StaticMesh::Release()
{
	// インデックスバッファ解放.
	if( m_ppIndexBuffer != nullptr ){
		for( int No = m_NumMaterials-1; No >= 0; No-- ){
			if( m_ppIndexBuffer[No] != nullptr ) SAFE_RELEASE( m_ppIndexBuffer[No] );
		}
		SAFE_DELETE_ARRAY( m_ppIndexBuffer );
	}
	SAFE_DELETE_ARRAY( m_pMaterials );
	SAFE_RELEASE( m_pMesh );
	SAFE_RELEASE( m_pSampleLinear );
	SAFE_RELEASE( m_pVertexBuffer );
	SAFE_RELEASE( m_pCBufferPerMaterial );
	SAFE_RELEASE( m_pCBufferPerMesh );
	SAFE_RELEASE( m_pPixelShader );
	SAFE_RELEASE( m_pVertexLayout );
	SAFE_RELEASE( m_pVertexShader );

	m_pDevice9		= nullptr;
	m_pContext11	= nullptr;
	m_pDevice11		= nullptr;
	m_hWnd			= nullptr;
}

//===========================================================
//	HLSLﾌｧｲﾙを読み込みｼｪｰﾀﾞを作成する.
//	HLSL: High Level Shading Language の略.
//===========================================================
HRESULT CDX9StaticMesh::InitShader()
{
	ID3DBlob* pCompiledShader = nullptr;
	ID3DBlob* pErrors = nullptr;
	UINT uCompileFlag = 0;
#ifdef _DEBUG
	uCompileFlag = D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
#endif	// #ifdef _DEBUG

	// HLSLから頂点シェーダーのブロブを作成.
	if( FAILED(
		D3DX11CompileFromFile(
			SHADER_VS_NAME, nullptr, nullptr, "VS_Main", "vs_5_0",
			uCompileFlag, 0, nullptr, &pCompiledShader, &pErrors, nullptr ))){
		_ASSERT_EXPR(false, L"hlsl読み込み失敗");
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);

	// 上記で作成したブロブから「頂点シェーダー」を作成.
	if( FAILED(
		m_pDevice11->CreateVertexShader(
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			nullptr,
			&m_pVertexShader ))){
		_ASSERT_EXPR(false, L"ﾊﾞｰﾃｯｸｽｼｪｰﾀﾞ作成失敗");
		return E_FAIL;
	}

	// 頂点インプットレイアウトを定義.
	// 頂点インプットレイアウトの配列要素数を算出.
	UINT numElements = 0;
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"NORMAL",  0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,   0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TANGENT", 0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"BINORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	};
	numElements = sizeof(layout) / sizeof(layout[0]);	// 要素数算出.

	// 頂点インプットレイアウトを作成.
	if( FAILED( m_pDevice11->CreateInputLayout(
			layout,
			numElements,
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			&m_pVertexLayout ))){
		_ASSERT_EXPR(false, L"頂点ｲﾝﾌﾟｯﾄﾚｲｱｳﾄ作成失敗");
		return E_FAIL;
	}
	SAFE_RELEASE(pCompiledShader);

	// HLSLからピクセルシェーダーのブロブを作成.
	if( FAILED(
		D3DX11CompileFromFile(
			SHADER_PS_NAME, nullptr, nullptr, "PS_Main", "ps_5_0",
			uCompileFlag, 0, nullptr, &pCompiledShader, &pErrors, nullptr ))){
		_ASSERT_EXPR(false, L"hlsl読み込み失敗");
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);

	// 上記で作成したブロブから「ピクセルシェーダー」を作成.
	if( FAILED(
		m_pDevice11->CreatePixelShader(
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			nullptr,
			&m_pPixelShader ))){
		_ASSERT_EXPR(false, L"ﾋﾟｸｾﾙｼｪｰﾀﾞ作成失敗");
		return E_FAIL;
	}
	SAFE_RELEASE(pCompiledShader);

	if( FAILED( shader::CreateConstantBuffer( m_pDevice11, &m_pCBufferPerMesh,		sizeof(CBUFFER_PER_MESH) )))	return E_FAIL;
	if( FAILED( shader::CreateConstantBuffer( m_pDevice11, &m_pCBufferPerMaterial,	sizeof(CBUFFER_PER_MATERIAL) )))return E_FAIL;
	if( FAILED( shader::CreateConstantBuffer( m_pDevice11, &m_pCBufferPerFrame,		sizeof(CBUFFER_PER_FRAME) )))	return E_FAIL;

	return S_OK;
}

// 描画関数.
void CDX9StaticMesh::Render()
{
	// ワールド行列.
	D3DXMATRIX mWorld = m_Tranceform.GetWorldMatrix();


	// 使用するシェーダーのセット.
	m_pContext11->VSSetShader( m_pVertexShader, nullptr, 0 );
	m_pContext11->PSSetShader( m_pPixelShader, nullptr, 0 );

	// シェーダーのコンスタントバッファに各種データを渡す.
	D3D11_MAPPED_SUBRESOURCE pData;
	if( SUCCEEDED( m_pContext11->Map(
		m_pCBufferPerFrame, 
		0,
		D3D11_MAP_WRITE_DISCARD,
		0, &pData ))) {
		CBUFFER_PER_FRAME cb;	// コンスタントバッファ.

		// カメラ位置.
		D3DXVECTOR3 camPos = CCameraManager::GetPosition();
		cb.vCamPos = D3DXVECTOR4( camPos.x, camPos.y, camPos.z, 1.0f );

		//----- ライト情報 -----.
		// ライト方向.
		D3DXVECTOR3 lightDir = CLightManager::GetDirection();
		cb.vLightDir = D3DXVECTOR4( lightDir.x, lightDir.y, lightDir.z, 1.0f );
		// ライト強度(明るさ).
		cb.vIntensity.x = CLightManager::GetIntensity();
		// ライト方向の正規化(ノーマライズ).
		D3DXVec4Normalize( &cb.vLightDir, &cb.vLightDir );

		memcpy_s( pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb) );

		// バッファ内のデータの書き換え終了時にUnmap.
		m_pContext11->Unmap(m_pCBufferPerFrame, 0);
	}

	m_pContext11->VSSetConstantBuffers( 2, 1, &m_pCBufferPerFrame);
	m_pContext11->PSSetConstantBuffers( 2, 1, &m_pCBufferPerFrame);

	D3DXMATRIX mView = CCameraManager::GetViewMatrix();
	D3DXMATRIX mProj = CCameraManager::GetProjMatrix();

	RenderMesh( mWorld, mView, mProj );
}

// メッシュの描画.
void CDX9StaticMesh::RenderMesh(
	D3DXMATRIX& mWorld, 
	const D3DXMATRIX& mView, 
	const D3DXMATRIX& mProj )
{
	// シェーダーのコンスタントバッファに各種データを渡す.
	D3D11_MAPPED_SUBRESOURCE pData;
	if( SUCCEEDED( m_pContext11->Map(
		m_pCBufferPerMesh, 
		0,
		D3D11_MAP_WRITE_DISCARD,
		0, &pData ))){

		CBUFFER_PER_MESH cb;	// コンスタントバッファ

		// ワールド行列を渡す.
		cb.mW = mWorld;
		D3DXMatrixTranspose( &cb.mW, &cb.mW );

		// ワールド・ビュー・プロジェクション行列を渡す.
		cb.mWVP = mWorld * mView * mProj;
		D3DXMatrixTranspose( &cb.mWVP, &cb.mWVP );

		// 色を渡す.
		cb.vColor = m_Color;

		memcpy_s( pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));

		// バッファ内のデータの書き換え終了時にUnmap.
		m_pContext11->Unmap( m_pCBufferPerMesh, 0 );
	}


	m_pContext11->VSSetConstantBuffers( 0, 1, &m_pCBufferPerMesh );
	m_pContext11->PSSetConstantBuffers( 0, 1, &m_pCBufferPerMesh );

	// 頂点インプットレイアウトをセット.
	m_pContext11->IASetInputLayout(m_pVertexLayout);

	// プリミティブ・トポロジーをセット.
	m_pContext11->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	// 頂点バッファをセット.
	UINT stride = m_pMesh->GetNumBytesPerVertex();
	UINT offset = 0;
	m_pContext11->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, &stride, &offset );

	// 属性の数だけ、それぞれの属性のインデックスバッファを描画.
	for( DWORD No = 0; No < m_NumAttr; No++ ){
		// 使用されていないマテリアル対策.
		if( m_pMaterials[m_AttrID[No]].dwNumFace == 0 ) continue;
		// インデックスバッファをセット.
		m_pContext11->IASetIndexBuffer( m_ppIndexBuffer[No], DXGI_FORMAT_R32_UINT, 0 );
		// マテリアルの各要素をシェーダーに渡す.
		D3D11_MAPPED_SUBRESOURCE pDataMat;
		if( SUCCEEDED( m_pContext11->Map(
			m_pCBufferPerMaterial,
			0, 
			D3D11_MAP_WRITE_DISCARD, 
			0, 
			&pDataMat ))){

			CBUFFER_PER_MATERIAL cb;

			// アンビエント,ディフューズｽﾞ,スペキュラシェーダーに渡す.
			cb.vAmbient = m_pMaterials[m_AttrID[No]].Ambient;
			cb.vDiffuse = m_pMaterials[m_AttrID[No]].Diffuse;
			cb.vSpecular = m_pMaterials[m_AttrID[No]].Specular;

			memcpy_s( pDataMat.pData, pDataMat.RowPitch, (void*)&cb, sizeof(cb) );

			m_pContext11->Unmap(m_pCBufferPerMaterial, 0);
		}

		m_pContext11->VSSetConstantBuffers( 1, 1, &m_pCBufferPerMaterial );
		m_pContext11->PSSetConstantBuffers( 1, 1, &m_pCBufferPerMaterial );

		m_pContext11->PSSetSamplers( 0, 1, &m_pSampleLinear );
		// テクスチャをシェーダーに渡す.
		ID3D11ShaderResourceView* pTex = { 0 };
		m_pContext11->PSSetShaderResources( 0, 1, &m_pMaterials[m_AttrID[No]].pTexture );
		m_pContext11->PSSetShaderResources( 1, 1, &m_pMaterials[m_AttrID[No]].pNormTexture );

		// ポリンゴンをレンダリング.
		m_pContext11->DrawIndexed( m_pMaterials[m_AttrID[No]].dwNumFace * 3, 0, 0);
		ID3D11ShaderResourceView* resetSrv = nullptr;
		m_pContext11->PSSetShaderResources( 0, 1, &resetSrv );
		m_pContext11->PSSetShaderResources( 1, 1, &resetSrv );
	}
}
