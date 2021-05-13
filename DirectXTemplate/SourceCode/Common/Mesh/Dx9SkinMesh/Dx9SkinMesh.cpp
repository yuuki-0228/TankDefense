/***************************************************************************************************
*	SkinMeshCode Version 2.00
*	LastUpdate	: 2019/10/09.
*		パーサークラスとスキンメッシュクラスを別ファイルに分割.
**/
#include "Dx9SkinMesh.h"
#include "..\..\..\Object\CameraBase\CameraManager\CameraManager.h"
#include "..\..\..\Object\LightBase\LightManager\LightManager.h"

//シェーダ名(ディレクトリも含む)
const char SHADER_VS_NAME[] = "Data\\Shader\\SkinMesh.hlsl";
const char SHADER_PS_NAME[] = "Data\\Shader\\MeshPS.hlsl";

/******************************************************************************************************************************************
*
*	以降、スキンメッシュクラス.
*
**/
//コンストラクタ.
CDX9SkinMesh::CDX9SkinMesh()
	: m_hWnd(nullptr)
	, m_pDevice9(nullptr)
	, m_pSampleLinear(nullptr)
	, m_pVertexShader(nullptr)
	, m_pPixelShader(nullptr)
	, m_pVertexLayout(nullptr)
	, m_pCBufferPerMesh(nullptr)
	, m_pCBufferPerMaterial(nullptr)
	, m_pCBufferPerFrame(nullptr)
	, m_pCBufferPerBone(nullptr)
	, m_mWorld()
	, m_mRotation()
	, m_mView()
	, m_mProj()
	, m_CameraPos()
	, m_CameraLookPos()
	, m_dAnimSpeed(0.0001f)	//一先ず、この値.
	, m_dAnimTime()
	, m_IsChangeAnim(false)
	, m_pReleaseMaterial(nullptr)
	, m_pD3dxMesh(nullptr)
	, m_FilePath()
	, m_iFrame()
{

}


//デストラクタ.
CDX9SkinMesh::~CDX9SkinMesh()
{
	//解放処理.
	Release();

	//シェーダやサンプラ関係.
	SAFE_RELEASE( m_pSampleLinear );
	SAFE_RELEASE( m_pVertexShader );
	SAFE_RELEASE( m_pPixelShader );
	SAFE_RELEASE( m_pVertexLayout );

	//コンスタントバッファ関係.
	SAFE_RELEASE(m_pCBufferPerBone);
	SAFE_RELEASE(m_pCBufferPerFrame);
	SAFE_RELEASE(m_pCBufferPerMaterial);
	SAFE_RELEASE(m_pCBufferPerMesh);

	m_pReleaseMaterial = nullptr;

	SAFE_RELEASE(m_pD3dxMesh);

	//Dx9 デバイス関係.
	m_pDevice9 = nullptr;

	//Dx11 デバイス関係.
	m_pContext11 = nullptr;
	m_pDevice11 = nullptr;

	m_hWnd = nullptr;
}


//初期化.
HRESULT CDX9SkinMesh::Init(
	HWND hWnd, 
	ID3D11Device* pDevice11,
	ID3D11DeviceContext* pContext11,
	LPDIRECT3DDEVICE9 pDevice9,
	const char* fileName )
{
	m_hWnd = hWnd;
	m_pDevice9 = pDevice9;
	if( FAILED( SetDevice( pContext11 ))) return E_FAIL;
	//シェーダの作成.
	if( FAILED( InitShader() ) ) return E_FAIL;
	
	//モデル読み込み.
	if( FAILED(LoadXMesh(fileName)))return E_FAIL;
	
	std::string msg = fileName;
	msg += " 読み込み : 成功";
	CLog::Print( std::string( msg.c_str() ).c_str() );

	return S_OK;
}

//シェーダ初期化.
HRESULT	CDX9SkinMesh::InitShader()
{
	//D3D11関連の初期化
	ID3DBlob *pCompiledShader = nullptr;
	ID3DBlob *pErrors = nullptr;
	UINT	uCompileFlag = 0;
#ifdef _DEBUG
	uCompileFlag = D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
#endif//#ifdef _DEBUG

	//ブロブからバーテックスシェーダー作成.
	if( FAILED(
		D3DX11CompileFromFile(
			SHADER_VS_NAME, nullptr, nullptr,
			"VS_Main", "vs_5_0",
			uCompileFlag, 0, nullptr,
			&pCompiledShader, &pErrors, nullptr ) ) )
	{
		int size = pErrors->GetBufferSize();
		char* ch = (char*)pErrors->GetBufferPointer();
		MessageBox( 0, "hlsl読み込み失敗", NULL, MB_OK );
		return E_FAIL;
	}
	SAFE_RELEASE( pErrors );

	if( FAILED(
		m_pDevice11->CreateVertexShader( pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), NULL, &m_pVertexShader ) ) )
	{
		SAFE_RELEASE(pCompiledShader);
		MessageBox( 0, "バーテックスシェーダー作成失敗", NULL, MB_OK );
		return E_FAIL;
	}
	//頂点インプットレイアウトを定義	
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
		{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONE_INDEX",	0, DXGI_FORMAT_R32G32B32A32_UINT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONE_WEIGHT",0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof( layout ) / sizeof( layout[0] );

	//頂点インプットレイアウトを作成
	if( FAILED(
		m_pDevice11->CreateInputLayout(
			layout, numElements, pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(), &m_pVertexLayout ) ) )
	{
		return FALSE;
	}
	//頂点インプットレイアウトをセット
	m_pContext11->IASetInputLayout( m_pVertexLayout );

	//ブロブからピクセルシェーダー作成
	if(FAILED(
		D3DX11CompileFromFile(
			SHADER_PS_NAME, nullptr, nullptr,
			"PS_Main", "ps_5_0",
			uCompileFlag, 0, nullptr,
			&pCompiledShader, &pErrors, nullptr ) ) )
	{
		MessageBox( 0, "hlsl読み込み失敗", NULL, MB_OK );
		return E_FAIL;
	}
	SAFE_RELEASE( pErrors );
	if( FAILED(
		m_pDevice11->CreatePixelShader(
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			NULL, &m_pPixelShader ) ) )
	{
		SAFE_RELEASE( pCompiledShader );
		MessageBox( 0, "ピクセルシェーダー作成失敗", NULL, MB_OK );
		return E_FAIL;
	}
	SAFE_RELEASE( pCompiledShader );

	//コンスタントバッファ(メッシュごと).
	if (FAILED(CreateCBuffer(&m_pCBufferPerMesh, sizeof(CBUFFER_PER_MESH))))	{	return E_FAIL;	}
	//コンスタントバッファ(メッシュごと).
	if (FAILED(CreateCBuffer(&m_pCBufferPerMaterial, sizeof(CBUFFER_PER_MATERIAL)))){	return E_FAIL;	}
	//コンスタントバッファ(メッシュごと).
	if (FAILED(CreateCBuffer(&m_pCBufferPerFrame, sizeof(CBUFFER_PER_FRAME))))	{	return E_FAIL;	}
	//コンスタントバッファ(メッシュごと).
	if (FAILED(CreateCBuffer(&m_pCBufferPerBone, sizeof(CBUFFER_PER_BONES))))	{	return E_FAIL;	}

	//テクスチャー用サンプラー作成.
	if (FAILED(CreateLinearSampler(&m_pSampleLinear))) {
		return E_FAIL;
	}

	return S_OK;
}



//Xファイルからスキン関連の情報を読み出す関数.
HRESULT CDX9SkinMesh::ReadSkinInfo(
	MYMESHCONTAINER* pContainer, MY_SKINVERTEX* pvVB, SKIN_PARTS_MESH* pParts )
{
	//Xファイルから抽出すべき情報は、
	//「頂点ごとのﾎﾞｰﾝｲﾝﾃﾞｯｸｽ」「頂点ごとのボーンウェイト」.
	//「バインド行列」「ポーズ行列」の4項目.

	int i, k, m, n;	//各種ループ変数.
	int iNumVertex	= 0;	//頂点数.
	int iNumBone	= 0;	//ボーン数.

	//頂点数.
	iNumVertex	= m_pD3dxMesh->GetNumVertices( pContainer );
	//ボーン数.
	iNumBone	= m_pD3dxMesh->GetNumBones( pContainer );

	//それぞれのボーンに影響を受ける頂点を調べる.
	//そこから逆に、頂点ベースでボーンインデックス・重みを整頓する.
	for( i=0; i<iNumBone; i++ )
	{
		//このボーンに影響を受ける頂点数.
		int iNumIndex
			= m_pD3dxMesh->GetNumBoneVertices( pContainer, i);

		int*	pIndex = new int[iNumIndex]();
		double*	pWeight= new double[iNumIndex]();

		for( k=0; k<iNumIndex; k++ )
		{
			pIndex[k]
				= m_pD3dxMesh->GetBoneVerticesIndices( pContainer, i, k );
			pWeight[k]
				= m_pD3dxMesh->GetBoneVerticesWeights( pContainer, i, k );
		}

		//頂点側からインデックスをたどって、頂点サイドで整理する.
		for( k=0; k<iNumIndex; k++ )
		{
			//XファイルやCGソフトがボーン4本以内とは限らない.
			//5本以上の場合は、重みの大きい順に4本に絞る.

			//ウェイトの大きさ順にソート(バブルソート).
			for( m=4; m>1; m-- )
			{
				for( n=1; n<m; n++ )
				{
					if( pvVB[pIndex[k]].bBoneWeight[n-1] < pvVB[pIndex[k]].bBoneWeight[n] )
					{
						float tmp = pvVB[pIndex[k]].bBoneWeight[n-1];
						pvVB[pIndex[k]].bBoneWeight[n-1] = pvVB[pIndex[k]].bBoneWeight[n];
						pvVB[pIndex[k]].bBoneWeight[n]	= tmp;
						int itmp = pvVB[pIndex[k]].bBoneIndex[n-1];
						pvVB[pIndex[k]].bBoneIndex[n-1] = pvVB[pIndex[k]].bBoneIndex[n];
						pvVB[pIndex[k]].bBoneIndex[n]	= itmp;
					}
				}
			}
			//ソート後は、最後の要素に一番小さいウェイトが入ってるはず.
			bool flag = false;
			for( m=0; m<4; m++ )
			{
				if( pvVB[pIndex[k]].bBoneWeight[m] == 0 )
				{
					flag = true;
					pvVB[pIndex[k]].bBoneIndex[ m] = i;
					pvVB[pIndex[k]].bBoneWeight[m] = (float)pWeight[k];
					break;
				}
			}
			if( flag == false )
			{
				pvVB[pIndex[k]].bBoneIndex[ 3] = i;
				pvVB[pIndex[k]].bBoneWeight[3] = (float)pWeight[k];
				break;
			}
		}
		//使い終われば削除する.
		delete[] pIndex;
		delete[] pWeight;
	}

	//ボーン生成.
	pParts->iNumBone	= iNumBone;
	pParts->pBoneArray	= new BONE[iNumBone]();
	//ポーズ行列を得る(初期ポーズ).
	for( i=0; i<pParts->iNumBone; i++ )
	{
		pParts->pBoneArray[i].mBindPose
			= m_pD3dxMesh->GetBindPose( pContainer, i );
	}

	return S_OK;
}

//Xからスキンメッシュを作成する.
//	注意）素材（X)のほうは、三角ポリゴンにすること.
HRESULT CDX9SkinMesh::LoadXMesh( const char* fileName )
{
	//ファイル名をパスごと取得.
	strcpy_s( m_FilePath, sizeof( m_FilePath ), fileName);

	//Xファイル読み込み.
	m_pD3dxMesh = new D3DXPARSER();
	m_pD3dxMesh->LoadMeshFromX( m_pDevice9, fileName);


	//全てのメッシュを作成する.
	BuildAllMesh( m_pD3dxMesh->m_pFrameRoot );

	return S_OK;
}


//Direct3Dのインデックスバッファー作成.
HRESULT CDX9SkinMesh::CreateIndexBuffer( DWORD dwSize, int* pIndex, ID3D11Buffer** ppIndexBuffer )
{
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = dwSize;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = pIndex;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	if( FAILED( m_pDevice11->CreateBuffer( &bd, &InitData, ppIndexBuffer ) ) )
	{
		return FALSE;
	}
	
	return S_OK;
}

//レンダリング.
void CDX9SkinMesh::Render( SAnimationController* pAC )
{
	m_mView		= CCameraManager::GetViewMatrix();
	m_mProj		= CCameraManager::GetProjMatrix();
	m_CameraPos = CCameraManager::GetPosition();
	m_CameraLookPos = CCameraManager::GetLookPosition();

	if (pAC == nullptr)
	{
		if (m_pD3dxMesh->m_pAnimController)
		{
			BlendAnimUpdate();
			m_pD3dxMesh->m_pAnimController->AdvanceTime(m_dAnimSpeed, NULL);
		}
	} else {
		pAC->BlendAnimUpdate( m_dAnimSpeed );
		pAC->pAC->AdvanceTime(m_dAnimSpeed, NULL);
	}

	D3DXMATRIX m;
	D3DXMatrixIdentity( &m );
	m_pD3dxMesh->UpdateFrameMatrices( m_pD3dxMesh->m_pFrameRoot, &m );
	DrawFrame( m_pD3dxMesh->m_pFrameRoot );
}


//全てのメッシュを作成する.
void CDX9SkinMesh::BuildAllMesh( D3DXFRAME* pFrame )
{
	if( pFrame && pFrame->pMeshContainer )
	{
		CreateAppMeshFromD3DXMesh( pFrame );
	}

	//再帰関数.
	if( pFrame->pFrameSibling != nullptr )
	{
		BuildAllMesh( pFrame->pFrameSibling );
	}
	if( pFrame->pFrameFirstChild != nullptr )
	{
		BuildAllMesh( pFrame->pFrameFirstChild );
	}
}

//メッシュ作成.
HRESULT CDX9SkinMesh::CreateAppMeshFromD3DXMesh( LPD3DXFRAME p )
{
	MYFRAME* pFrame = (MYFRAME*)p;

//	LPD3DXMESH pD3DXMesh = pFrame->pMeshContainer->MeshData.pMesh;//D3DXﾒｯｼｭ(ここから・・・).
	m_pMeshForRay= pFrame->pMeshContainer->MeshData.pMesh; //D3DXﾒｯｼｭ(ここから・・・).
	MYMESHCONTAINER* pContainer = (MYMESHCONTAINER*)pFrame->pMeshContainer;

	//アプリメッシュ(・・・ここにメッシュデータをコピーする).
	SKIN_PARTS_MESH* pAppMesh = new SKIN_PARTS_MESH();
	pAppMesh->bTex = false;

	//事前に頂点数、ポリゴン数等を調べる.
	pAppMesh->dwNumVert	= m_pD3dxMesh->GetNumVertices( pContainer );
	pAppMesh->dwNumFace	= m_pD3dxMesh->GetNumFaces( pContainer );
	pAppMesh->dwNumUV	= m_pD3dxMesh->GetNumUVs( pContainer );
	//Direct3DではUVの数だけ頂点が必要.
	if( pAppMesh->dwNumVert < pAppMesh->dwNumUV ){
		//共有頂点等で、頂点が足りないとき.
		MessageBox( NULL,
			"Direct3Dは、UVの数だけ頂点が必要です(UVを置く場所が必要です)テクスチャは正しく貼られないと思われます",
			"Error", MB_OK );
		return E_FAIL;
	}
	//一時的なメモリ確保(頂点バッファとインデックスバッファ).
	MY_SKINVERTEX* pvVB = new MY_SKINVERTEX[pAppMesh->dwNumVert]();
	int* piFaceBuffer	= new int[pAppMesh->dwNumFace*3]();
	//3頂点ポリゴンなので、1フェイス=3頂点(3インデックス).

	//頂点読み込み.
	for( DWORD i=0; i<pAppMesh->dwNumVert; i++ ){
		D3DXVECTOR3 v	= m_pD3dxMesh->GetVertexCoord( pContainer, i );
		pvVB[i].vPos.x	= v.x;
		pvVB[i].vPos.y	= v.y;
		pvVB[i].vPos.z	= v.z;
	}
	//ポリゴン情報(頂点インデックス)読み込み.
	for( DWORD i=0; i<pAppMesh->dwNumFace*3; i++ ){
		piFaceBuffer[i] = m_pD3dxMesh->GetIndex( pContainer, i );
	}
	//法線読み込み.
	for( DWORD i=0; i<pAppMesh->dwNumVert; i++ ){
		D3DXVECTOR3 v	= m_pD3dxMesh->GetNormal( pContainer, i );
		pvVB[i].vNorm.x	= v.x;
		pvVB[i].vNorm.y	= v.y;
		pvVB[i].vNorm.z	= v.z;
	}
	//テクスチャ座標読み込み.
	for( DWORD i=0; i<pAppMesh->dwNumVert; i++ ){
		D3DXVECTOR2 v	= m_pD3dxMesh->GetUV( pContainer, i );
		pvVB[i].vTex.x	= v.x;
		pvVB[i].vTex.y	= v.y;
	}
	//tangent.
	for( DWORD i=0; i<pAppMesh->dwNumVert; i++ ){
		D3DXVECTOR3 v	= m_pD3dxMesh->GetTangent( pContainer, i );
		pvVB[i].Tangent.x	= v.x;
		pvVB[i].Tangent.y	= v.y;
		pvVB[i].Tangent.z	= v.z;
	}
	//binorm.
	for( DWORD i=0; i<pAppMesh->dwNumVert; i++ ){
		D3DXVECTOR3 v	= m_pD3dxMesh->GetBinormal( pContainer, i );
		pvVB[i].Binormal.x	= v.x;
		pvVB[i].Binormal.y	= v.y;
		pvVB[i].Binormal.z	= v.z;
	}
	//マテリアル読み込み.
	pAppMesh->dwNumMaterial	= m_pD3dxMesh->GetNumMaterials( pContainer );
	pAppMesh->pMaterial		= new MY_SKINMATERIAL[pAppMesh->dwNumMaterial]();

	//マテリアルの数だけインデックスバッファを作成.
	pAppMesh->ppIndexBuffer = new ID3D11Buffer*[pAppMesh->dwNumMaterial]();
	//掛け算ではなく「ID3D11Buffer*」の配列という意味.
	for( DWORD i=0; i<pAppMesh->dwNumMaterial; i++ )
	{
		//環境光(アンビエント).
		pAppMesh->pMaterial[i].Ka.x	= m_pD3dxMesh->GetAmbient( pContainer, i ).y;
		pAppMesh->pMaterial[i].Ka.y	= m_pD3dxMesh->GetAmbient( pContainer, i ).z;
		pAppMesh->pMaterial[i].Ka.z	= m_pD3dxMesh->GetAmbient( pContainer, i ).w;
		pAppMesh->pMaterial[i].Ka.w	= m_pD3dxMesh->GetAmbient( pContainer, i ).x;
		//拡散反射光(ディフューズ).
		pAppMesh->pMaterial[i].Kd.x	= m_pD3dxMesh->GetDiffuse( pContainer, i ).y;
		pAppMesh->pMaterial[i].Kd.y	= m_pD3dxMesh->GetDiffuse( pContainer, i ).z;
		pAppMesh->pMaterial[i].Kd.z	= m_pD3dxMesh->GetDiffuse( pContainer, i ).w;
		pAppMesh->pMaterial[i].Kd.w	= m_pD3dxMesh->GetDiffuse( pContainer, i ).x;
		//鏡面反射光(スペキュラ).
		pAppMesh->pMaterial[i].Ks.x	= m_pD3dxMesh->GetSpecular( pContainer, i ).y;
		pAppMesh->pMaterial[i].Ks.y	= m_pD3dxMesh->GetSpecular( pContainer, i ).z;
		pAppMesh->pMaterial[i].Ks.z	= m_pD3dxMesh->GetSpecular( pContainer, i ).w;
		pAppMesh->pMaterial[i].Ks.w	= m_pD3dxMesh->GetSpecular( pContainer, i ).x;

		//テクスチャ(ディフューズテクスチャのみ).
		char* name = m_pD3dxMesh->GetTexturePath( pContainer, i );
		if( name ){
			char* ret = strrchr( m_FilePath, '\\' );
			if( ret != NULL ){
				int check = ret - m_FilePath;
				char path[512];
				strcpy_s( path, 512, m_FilePath );
				path[check+1] = '\0';

				strcat_s( path, sizeof( path ), name );
				strcpy_s( pAppMesh->pMaterial[i].TextureName,
					sizeof( pAppMesh->pMaterial[i].TextureName ),
					path );
			}
		}
		//テクスチャを作成.
		if( lstrlen(pAppMesh->pMaterial[i].TextureName) > 0){
			if( FAILED( D3DX11CreateShaderResourceViewFromFile(
				m_pDevice11,
				pAppMesh->pMaterial[i].
				TextureName,
				NULL, 
				NULL, 
				&pAppMesh->pMaterial[i].pTexture, 
				NULL )))
			{
				MessageBox( NULL, "テクスチャ読み込み失敗",
					"Error", MB_OK );
				return E_FAIL;
			}
			// 法線テクスチャを取得.
			std::string normTexName = pAppMesh->pMaterial[i].TextureName;
			size_t normlIndex = normTexName.find(".");
			normTexName.insert( normlIndex , "_norm" );
			if( FAILED( D3DX11CreateShaderResourceViewFromFile(
				m_pDevice11, 
				normTexName.c_str(),	// テクスチャファイル名.
				nullptr, 
				nullptr,
				&pAppMesh->pMaterial[i].pNormTexture,		// (out)テクスチャオブジェクト.
				nullptr ))){
				pAppMesh->pMaterial[i].pNormTexture = nullptr;
			}
		} else {
			std::string text = m_FilePath;
			// テクスチャ作成.
			if( FAILED( D3DX11CreateShaderResourceViewFromFile(
				m_pDevice11, 
				"Data\\Mesh\\noTex.png",	// テクスチャファイル名.
				nullptr, 
				nullptr,
				&pAppMesh->pMaterial[i].pTexture,	// (out)テクスチャオブジェクト.
				nullptr ))){
				ERROR_MESSAGE( text + " テクスチャ読込 : 失敗" );
				return E_FAIL;
			}
			text += "にテクスチャがないため別のテクスチャを差し込みます。";
			CLog::Print( text.c_str() );
		}
		//そのマテリアルであるインデックス配列内の開始インデックスを調べる.
		//さらにインデックスの個数を調べる.
		int iCount = 0;
		int* pIndex = new int[pAppMesh->dwNumFace*3]();
			//とりあえず、メッシュ内のポリゴン数でメモリ確保.
			//(ここのぽりごんぐるーぷは必ずこれ以下になる).

		for( DWORD k=0; k<pAppMesh->dwNumFace; k++ )
		{
			//もし i==k 番目のポリゴンのマテリアル番号なら.
			if( i == m_pD3dxMesh->GeFaceMaterialIndex( pContainer, k ) )
			{
				//k番目のポリゴンを作る頂点のインデックス.
				pIndex[iCount]
					= m_pD3dxMesh->GetFaceVertexIndex( pContainer, k, 0 );	//1個目.
				pIndex[iCount+1]
					= m_pD3dxMesh->GetFaceVertexIndex( pContainer, k, 1 );	//2個目.
				pIndex[iCount+2]
					= m_pD3dxMesh->GetFaceVertexIndex( pContainer, k, 2 );	//3個目.
				iCount += 3;
			}
		}
		if( iCount > 0 ){
			//インデックスバッファ作成.
			CreateIndexBuffer( iCount*sizeof(int),
				pIndex, &pAppMesh->ppIndexBuffer[i] );
		}
		else{
			//解放時の処理に不具合が出たため.
			//カウント数が0以下の場合は、インデックスバッファを nullptr にしておく.
			pAppMesh->ppIndexBuffer[i] = nullptr;
		}

		//そのマテリアル内のポリゴン数.
		pAppMesh->pMaterial[i].dwNumFace = iCount / 3;
		//不要になったので削除.
		delete[] pIndex;
	}

	//スキン情報ある？
	if( pContainer->pSkinInfo == nullptr ){
	/*	char strDbg[128];
		sprintf_s( strDbg, "ContainerName:[%s]", pContainer->Name );
		MessageBox( nullptr, strDbg, "Not SkinInfo", MB_OK );*/
		pAppMesh->bEnableBones = false;
	}
	else{
		//スキン情報(ジョイント、ウェイト)読み込み.
		ReadSkinInfo( pContainer, pvVB, pAppMesh );
	}

	//バーテックスバッファを作成.
	D3D11_BUFFER_DESC bd;
	bd.Usage	= D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( MY_SKINVERTEX ) * ( pAppMesh->dwNumVert );
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags= 0;
	bd.MiscFlags	= 0;
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = pvVB;

	HRESULT hRslt = S_OK;
	if( FAILED(
		m_pDevice11->CreateBuffer(
			&bd, &InitData, &pAppMesh->pVertexBuffer ) ) )
	{
		hRslt = E_FAIL;
	}

	//パーツメッシュに設定.
	pFrame->pPartsMesh = pAppMesh;
	m_pReleaseMaterial = pAppMesh;

	//一時的な入れ物は不要なるので削除.
	if( piFaceBuffer ){
		delete[] piFaceBuffer;
	}
	if( pvVB ){
		delete[] pvVB;
	}

	return hRslt;
}


//ボーンを次のポーズ位置にセットする関数.
void CDX9SkinMesh::SetNewPoseMatrices(
	SKIN_PARTS_MESH* pParts, int frame, MYMESHCONTAINER* pContainer )
{
	//望むフレームでUpdateすること.
	//しないと行列が更新されない.
	//m_pD3dxMesh->UpdateFrameMatrices(
	// m_pD3dxMesh->m_pFrameRoot)をレンダリング時に実行すること.

	//また、アニメーション時間に見合った行列を更新するのはD3DXMESHでは
	//アニメーションコントローラが(裏で)やってくれるものなので、
	//アニメーションコントローラを使ってアニメを進行させることも必要.
	//m_pD3dxMesh->m_pAnimController->AdvanceTime(...)を.
	//レンダリング時に実行すること.

	if( pParts->iNumBone <= 0 ){
		//ボーンが 0　以下の場合.
	}

	for( int i=0; i<pParts->iNumBone; i++ )
	{
		pParts->pBoneArray[i].mNewPose
			= m_pD3dxMesh->GetNewPose( pContainer, i );
	}
}


//次の(現在の)ポーズ行列を返す関数.
D3DXMATRIX CDX9SkinMesh::GetCurrentPoseMatrix( SKIN_PARTS_MESH* pParts, int index )
{
	D3DXMATRIX ret =
		pParts->pBoneArray[index].mBindPose * pParts->pBoneArray[index].mNewPose;
	return ret;
}


//フレームの描画.
VOID CDX9SkinMesh::DrawFrame( LPD3DXFRAME p )
{
	MYFRAME*			pFrame	= (MYFRAME*)p;
	SKIN_PARTS_MESH*	pPartsMesh	= pFrame->pPartsMesh;
	MYMESHCONTAINER*	pContainer	= (MYMESHCONTAINER*)pFrame->pMeshContainer;

	if( pPartsMesh != nullptr )
	{
		DrawPartsMesh(
			pPartsMesh, 
			pFrame->CombinedTransformationMatrix,
			pContainer );
	}

	//再帰関数.
	//(兄弟)
	if( pFrame->pFrameSibling != nullptr )
	{
		DrawFrame( pFrame->pFrameSibling );
	}
	//(親子)
	if( pFrame->pFrameFirstChild != nullptr )
	{
		DrawFrame( pFrame->pFrameFirstChild );
	}
}


//パーツメッシュを描画.
void CDX9SkinMesh::DrawPartsMesh( SKIN_PARTS_MESH* pMesh, D3DXMATRIX World, MYMESHCONTAINER* pContainer )
{
	D3D11_MAPPED_SUBRESOURCE pData;

	//使用するシェーダのセット.
	m_pContext11->VSSetShader( m_pVertexShader, nullptr, 0 );
	m_pContext11->PSSetShader( m_pPixelShader, nullptr, 0 );

	//ワールド行列.
	m_mWorld = m_Tranceform.GetWorldMatrix();


	//アニメーションフレームを進める スキンを更新.
	m_iFrame++;
	if( m_iFrame >= 3600 ){
		m_iFrame = 0;
	}
	SetNewPoseMatrices( pMesh, m_iFrame, pContainer );

	//------------------------------------------------.
	//	コンスタントバッファに情報を送る(ボーン).
	//------------------------------------------------.
	if( SUCCEEDED(
		m_pContext11->Map(
			m_pCBufferPerBone, 0,
			D3D11_MAP_WRITE_DISCARD, 0, &pData ) ) )
	{
		CBUFFER_PER_BONES cb;
		for( int i=0; i<pMesh->iNumBone; i++ )
		{
			D3DXMATRIX mat = GetCurrentPoseMatrix( pMesh, i );
			D3DXMatrixTranspose( &mat, &mat );
			cb.mBone[i] = mat;
		}
		memcpy_s( pData.pData, pData.RowPitch, (void*)&cb, sizeof( cb ) );
		m_pContext11->Unmap(m_pCBufferPerBone, 0 );
	}
	m_pContext11->VSSetConstantBuffers(	3, 1, &m_pCBufferPerBone);
	m_pContext11->PSSetConstantBuffers(	3, 1, &m_pCBufferPerBone);
	
	//バーテックスバッファをセット.
	UINT stride = sizeof( MY_SKINVERTEX );
	UINT offset = 0;
	m_pContext11->IASetVertexBuffers(
		0, 1, &pMesh->pVertexBuffer, &stride, & offset );

	//頂点インプットレイアウトをセット.
	m_pContext11->IASetInputLayout(	m_pVertexLayout );

	//プリミティブ・トポロジーをセット.
	m_pContext11->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	//------------------------------------------------.
	//	コンスタントバッファに情報を設定(フレームごと).
	//------------------------------------------------.
	if( SUCCEEDED(
		m_pContext11->Map(
			m_pCBufferPerFrame, 0,
			D3D11_MAP_WRITE_DISCARD, 0, &pData ) ) )
	{
		CBUFFER_PER_FRAME cb;

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

		memcpy_s( pData.pData, pData.RowPitch, (void*)&cb, sizeof(cb) );
		m_pContext11->Unmap(m_pCBufferPerFrame, 0 );
	}
	m_pContext11->VSSetConstantBuffers(	2, 1, &m_pCBufferPerFrame);
	m_pContext11->PSSetConstantBuffers(	2, 1, &m_pCBufferPerFrame);


	//------------------------------------------------.
	//	コンスタントバッファに情報を設定(メッシュごと).
	//------------------------------------------------.
	D3D11_MAPPED_SUBRESOURCE pDat;
	if (SUCCEEDED(
		m_pContext11->Map(
			m_pCBufferPerMesh, 0,
			D3D11_MAP_WRITE_DISCARD, 0, &pDat)))
	{
		CBUFFER_PER_MESH cb;
		// ワールド行列を渡す.
		cb.mW = m_mWorld;
		D3DXMatrixTranspose( &cb.mW, &cb.mW );

		// ワールド・ビュー・プロジェクション行列を渡す.
		cb.mWVP = m_mWorld * m_mView * m_mProj;
		D3DXMatrixTranspose( &cb.mWVP, &cb.mWVP );

		// 色を渡す.
		cb.vColor = m_Color;

		memcpy_s(pDat.pData, pDat.RowPitch, (void*)&cb, sizeof(cb));
		m_pContext11->Unmap(m_pCBufferPerMesh, 0);
	}
	m_pContext11->VSSetConstantBuffers(0, 1, &m_pCBufferPerMesh);
	m_pContext11->PSSetConstantBuffers(0, 1, &m_pCBufferPerMesh);


	//マテリアルの数だけ、
	//それぞれのマテリアルのインデックスバッファを描画.
	for( DWORD i=0; i<pMesh->dwNumMaterial; i++ )
	{
		//使用されていないマテリアル対策.
		if( pMesh->pMaterial[i].dwNumFace == 0 )
		{
			continue;
		}
		//インデックスバッファをセット.
		stride	= sizeof( int );
		offset	= 0;
		m_pContext11->IASetIndexBuffer(
			pMesh->ppIndexBuffer[i],
			DXGI_FORMAT_R32_UINT, 0 );

		//------------------------------------------------.
		//	マテリアルの各要素と変換行列をシェーダに渡す.
		//------------------------------------------------.
		D3D11_MAPPED_SUBRESOURCE pDat;
		if( SUCCEEDED(
			m_pContext11->Map(
				m_pCBufferPerMaterial, 0,
				D3D11_MAP_WRITE_DISCARD, 0, &pDat )))
		{
			CBUFFER_PER_MATERIAL cb;
			cb.vAmbient	= pMesh->pMaterial[i].Ka;
			cb.vDiffuse	= pMesh->pMaterial[i].Kd;
			cb.vSpecular= pMesh->pMaterial[i].Ks;
			memcpy_s( pDat.pData, pDat.RowPitch, (void*)&cb, sizeof(cb));
			m_pContext11->Unmap( m_pCBufferPerMaterial, 0 );
		}
		m_pContext11->VSSetConstantBuffers(	1, 1, &m_pCBufferPerMaterial);
		m_pContext11->PSSetConstantBuffers(	1, 1, &m_pCBufferPerMaterial);

		m_pContext11->PSSetSamplers( 0, 1, &m_pSampleLinear );
		m_pContext11->PSSetShaderResources( 0, 1, &pMesh->pMaterial[i].pTexture );
		m_pContext11->PSSetShaderResources( 1, 1, &pMesh->pMaterial[i].pNormTexture );

		//Draw.
		m_pContext11->DrawIndexed( pMesh->pMaterial[i].dwNumFace * 3, 0, 0 );
	}
	ID3D11ShaderResourceView* resetSrv = nullptr;
	m_pContext11->PSSetShaderResources( 0, 1, &resetSrv );
	m_pContext11->PSSetShaderResources( 1, 1, &resetSrv );
}



//解放関数.
HRESULT CDX9SkinMesh::Release()
{
	if( m_pD3dxMesh != nullptr ){
		//全てのメッシュ削除.
		DestroyAllMesh( m_pD3dxMesh->m_pFrameRoot );

		//パーサークラス解放処理.
		m_pD3dxMesh->Release();
		SAFE_DELETE( m_pD3dxMesh );
	}

	return S_OK;
}


//全てのメッシュを削除.
void CDX9SkinMesh::DestroyAllMesh( D3DXFRAME* pFrame )
{
	if ((pFrame != nullptr) && (pFrame->pMeshContainer != nullptr))
	{
		DestroyAppMeshFromD3DXMesh( pFrame );
	}

	//再帰関数.
	if( pFrame->pFrameSibling != nullptr )
	{
		DestroyAllMesh( pFrame->pFrameSibling );
	}
	if( pFrame->pFrameFirstChild != nullptr )
	{
		DestroyAllMesh( pFrame->pFrameFirstChild );
	}
}


//メッシュの削除.
HRESULT CDX9SkinMesh::DestroyAppMeshFromD3DXMesh( LPD3DXFRAME p )
{
	MYFRAME* pFrame = (MYFRAME*)p;

	MYMESHCONTAINER* pMeshContainerTmp = (MYMESHCONTAINER*)pFrame->pMeshContainer;

	//MYMESHCONTAINERの中身の解放.
	if (pMeshContainerTmp != nullptr)
	{
		if (pMeshContainerTmp->pBoneBuffer != nullptr)
		{
			pMeshContainerTmp->pBoneBuffer->Release();
			pMeshContainerTmp->pBoneBuffer = nullptr;
		}

		if (pMeshContainerTmp->pBoneOffsetMatrices != nullptr)
		{
			delete pMeshContainerTmp->pBoneOffsetMatrices;
			pMeshContainerTmp->pBoneOffsetMatrices = nullptr;
		}
		
		if (pMeshContainerTmp->ppBoneMatrix != nullptr)
		{
			int iMax = static_cast<int>(pMeshContainerTmp->pSkinInfo->GetNumBones());

			for (int i = iMax - 1; i >= 0; i--)
			{
				if (pMeshContainerTmp->ppBoneMatrix[i] != nullptr)
				{
					pMeshContainerTmp->ppBoneMatrix[i] = nullptr;
				}
			}

			delete[] pMeshContainerTmp->ppBoneMatrix;
			pMeshContainerTmp->ppBoneMatrix = nullptr;
		}

		if (pMeshContainerTmp->ppTextures != nullptr)
		{
			int iMax = static_cast<int>(pMeshContainerTmp->NumMaterials);

			for (int i = iMax - 1; i >= 0; i--)
			{
				if (pMeshContainerTmp->ppTextures[i] != nullptr)
				{
					pMeshContainerTmp->ppTextures[i]->Release();
					pMeshContainerTmp->ppTextures[i] = nullptr;
				}
			}

			delete[] pMeshContainerTmp->ppTextures;
			pMeshContainerTmp->ppTextures = nullptr;
		}
	}

	pMeshContainerTmp = nullptr;

	//MYMESHCONTAINER解放完了.

	//LPD3DXMESHCONTAINERの解放.
	if (pFrame->pMeshContainer->pAdjacency != nullptr)
	{
		delete[] pFrame->pMeshContainer->pAdjacency;
		pFrame->pMeshContainer->pAdjacency = nullptr;
	}

	if (pFrame->pMeshContainer->pEffects != nullptr)
	{
		if (pFrame->pMeshContainer->pEffects->pDefaults != nullptr)
		{
			if (pFrame->pMeshContainer->pEffects->pDefaults->pParamName != nullptr)
			{
				delete pFrame->pMeshContainer->pEffects->pDefaults->pParamName;
				pFrame->pMeshContainer->pEffects->pDefaults->pParamName = nullptr;
			}

			if (pFrame->pMeshContainer->pEffects->pDefaults->pValue != nullptr)
			{
				delete pFrame->pMeshContainer->pEffects->pDefaults->pValue;
				pFrame->pMeshContainer->pEffects->pDefaults->pValue = nullptr;
			}

			delete pFrame->pMeshContainer->pEffects->pDefaults;
			pFrame->pMeshContainer->pEffects->pDefaults = nullptr;
		}

		if (pFrame->pMeshContainer->pEffects->pEffectFilename != nullptr)
		{
			delete pFrame->pMeshContainer->pEffects->pEffectFilename;
			pFrame->pMeshContainer->pEffects->pEffectFilename = nullptr;
		}

		delete pFrame->pMeshContainer->pEffects;
		pFrame->pMeshContainer->pEffects = nullptr;
	}

	if (pFrame->pMeshContainer->pMaterials != nullptr)
	{
		int iMax = static_cast<int>(pFrame->pMeshContainer->NumMaterials);

		for (int i = iMax - 1; i >= 0; i--)
		{
			delete[] pFrame->pMeshContainer->pMaterials[i].pTextureFilename;
			pFrame->pMeshContainer->pMaterials[i].pTextureFilename = nullptr;
		}

		delete[] pFrame->pMeshContainer->pMaterials;
		pFrame->pMeshContainer->pMaterials = nullptr;
	}

	if (pFrame->pMeshContainer->pNextMeshContainer != nullptr)
	{
		//次のメッシュコンテナーのポインターを持つのだとしたら.
		//newで作られることはないはずなので、これで行けると思う.
		pFrame->pMeshContainer->pNextMeshContainer = nullptr;
	}

	if (pFrame->pMeshContainer->pSkinInfo != nullptr)
	{
		pFrame->pMeshContainer->pSkinInfo->Release();
		pFrame->pMeshContainer->pSkinInfo = nullptr;
	}

	//LPD3DXMESHCONTAINERの解放完了.

	//MYFRAMEの解放.

	if (pFrame->pPartsMesh != nullptr)
	{
		//ボーン情報の解放.
		if (pFrame->pPartsMesh->pBoneArray)
		{
			delete[] pFrame->pPartsMesh->pBoneArray;
			pFrame->pPartsMesh->pBoneArray = nullptr;
		}

		if (pFrame->pPartsMesh->pMaterial != nullptr)
		{
			int iMax = static_cast<int>(pFrame->pPartsMesh->dwNumMaterial);

			for (int i = iMax - 1; i >= 0; i--)
			{
				if (pFrame->pPartsMesh->pMaterial[i].pTexture)
				{
					pFrame->pPartsMesh->pMaterial[i].pTexture->Release();
					pFrame->pPartsMesh->pMaterial[i].pTexture = nullptr;
				}
			}

			delete[] pFrame->pPartsMesh->pMaterial;
			pFrame->pPartsMesh->pMaterial = nullptr;
		}
		

		if (pFrame->pPartsMesh->ppIndexBuffer)
		{
			//インデックスバッファ解放.
			for (DWORD i = 0; i<pFrame->pPartsMesh->dwNumMaterial; i++){
				if (pFrame->pPartsMesh->ppIndexBuffer[i] != nullptr){
					pFrame->pPartsMesh->ppIndexBuffer[i]->Release();
					pFrame->pPartsMesh->ppIndexBuffer[i] = nullptr;
				}
			}
			delete[] pFrame->pPartsMesh->ppIndexBuffer;
		}

		//頂点バッファ開放.
		pFrame->pPartsMesh->pVertexBuffer->Release();
		pFrame->pPartsMesh->pVertexBuffer = nullptr;
	}

	//パーツマテリアル開放.
	delete[] pFrame->pPartsMesh;
	pFrame->pPartsMesh = nullptr;

	//SKIN_PARTS_MESH解放完了.

	//MYFRAMEのSKIN_PARTS_MESH以外のメンバーポインター変数は別の関数で解放されていました.

	return S_OK;
}



//アニメーションセットの切り替え.
void CDX9SkinMesh::ChangeAnimSet( int index, SAnimationController* pAC )
{
	if( m_pD3dxMesh == nullptr )	return;
	m_pD3dxMesh->ChangeAnimSet( index, pAC == nullptr ? nullptr : pAC->pAC );
	if( pAC != nullptr ){
		pAC->IsAnimChangeBlend = false;
		pAC->AnimTime = 0.0;
	}
	m_IsChangeAnim = false;
	m_dAnimTime = 0.0;
}


//アニメーションセットの切り替え(開始フレーム指定可能版).
void CDX9SkinMesh::ChangeAnimSet_StartPos( int index, double dStartFramePos, SAnimationController* pAC )
{
	if( m_pD3dxMesh == nullptr )	return;
	m_pD3dxMesh->ChangeAnimSet_StartPos( index, dStartFramePos, pAC == nullptr ? nullptr : pAC->pAC );
}

// アニメーションをブレンドして切り替え.
void CDX9SkinMesh::ChangeAnimBlend( int index, int oldIndex, SAnimationController* pAC )
{
	if( m_pD3dxMesh == nullptr )	return;
	m_pD3dxMesh->ChangeAnimBlend( index, oldIndex, pAC == nullptr ? nullptr : pAC->pAC );
	if( pAC != nullptr ){
		pAC->IsAnimChangeBlend = true;
		pAC->AnimTime = 0.0;
	}
	m_IsChangeAnim = true;
	m_dAnimTime = 0.0;
}

// ブレンドアニメーションの更新.
void CDX9SkinMesh::BlendAnimUpdate()
{
	// アニメーション切り替えフラグが下りてたら終了.
	if( m_IsChangeAnim == false ) return;

	m_dAnimTime += m_dAnimSpeed;
	float weight = static_cast<float>(m_dAnimTime) / 1.0f;	// ウェイトの計算.
	m_pD3dxMesh->m_pAnimController->SetTrackWeight( 0, weight );	// トラック0 にウェイトを設定.
	m_pD3dxMesh->m_pAnimController->SetTrackWeight( 1, 1 - weight );// トラック1 にウェイトを設定.

	if( m_dAnimTime < 1.0 ) return;
	// アニメーションタイムが一定値に達したら.
	m_IsChangeAnim = false;	// フラグを下す.
	m_pD3dxMesh->m_pAnimController->SetTrackWeight( 0, 1.0f );		// ウェイトを1に固定する.
	m_pD3dxMesh->m_pAnimController->SetTrackEnable( 1, false );		// トラック1を無効にする.
}

//アニメーション停止時間を取得.
double CDX9SkinMesh::GetAnimPeriod( int index )
{
	if( m_pD3dxMesh == nullptr ){
		return 0.0f;
	}
	return m_pD3dxMesh->GetAnimPeriod( index );
}


//アニメーション数を取得.
int CDX9SkinMesh::GetAnimMax( LPD3DXANIMATIONCONTROLLER pAC )
{
	if( m_pD3dxMesh != nullptr ){
		return m_pD3dxMesh->GetAnimMax( pAC );
	}
	return -1;
}


//指定したボーン情報(行列)を取得する関数.
bool CDX9SkinMesh::GetMatrixFromBone(const char* sBoneName, D3DXMATRIX* pOutMat )
{
	if( m_pD3dxMesh != nullptr ){
		if( m_pD3dxMesh->GetMatrixFromBone( sBoneName, pOutMat ) ){
			return true;
		}
	}
	return false;
}
//指定したボーン情報(座標)を取得する関数.
bool CDX9SkinMesh::GetPosFromBone(const char* sBoneName, D3DXVECTOR3* pOutPos)
{
	if( m_pD3dxMesh != nullptr ){
		D3DXVECTOR3 tmpPos;
		if( m_pD3dxMesh->GetPosFromBone( sBoneName, &tmpPos ) ){
			D3DXMATRIX mWorld, mScale, mTran;
			D3DXMATRIX mRot, mYaw, mPitch, mRoll;
			D3DXMatrixScaling( &mScale, m_Tranceform.Scale.x, m_Tranceform.Scale.y, m_Tranceform.Scale.z );
			D3DXMatrixRotationY( &mYaw, m_Tranceform.Rotation.y);
			D3DXMatrixRotationX( &mPitch, m_Tranceform.Rotation.x);
			D3DXMatrixRotationZ( &mRoll, m_Tranceform.Rotation.z);
			D3DXMatrixTranslation(&mTran, tmpPos.x, tmpPos.y, tmpPos.z);

			mRot = mYaw * mPitch * mRoll;
			mWorld = mTran * mScale * mRot;

			pOutPos->x = mWorld._41 + m_Tranceform.Position.x;
			pOutPos->y = mWorld._42 + m_Tranceform.Position.y;
			pOutPos->z = mWorld._43 + m_Tranceform.Position.z;

			return true;
		}
	}
	return false;
}

//指定したボーン情報(行列)を設定する関数.
bool CDX9SkinMesh::SetMatrixFromBone(const char* sBoneName, D3DXMATRIX inMat )
{
	if( m_pD3dxMesh != nullptr ){
		if( m_pD3dxMesh->SetMatrixFromBone( sBoneName, inMat ) ){
			return true;
		}
	}
	return false;
}
//指定したボーン情報(座標)を設定する関数.
bool CDX9SkinMesh::SetPosFromBone(const char* sBoneName, D3DXVECTOR3 inPos)
{
	if( m_pD3dxMesh != nullptr ){
		if( m_pD3dxMesh->SetPosFromBone( sBoneName, inPos ) ){
			return true;
		}
	}
	return false;
}

bool CDX9SkinMesh::GetDeviaPosFromBone(const char* sBoneName, D3DXVECTOR3* pOutPos, D3DXVECTOR3 vSpecifiedPos)
{
	if (m_pD3dxMesh != nullptr){
		D3DXMATRIX mtmp;
		if (m_pD3dxMesh->GetMatrixFromBone(sBoneName, &mtmp)){
			D3DXMATRIX mWorld, mScale, mTran, mDevia;
			D3DXMATRIX mRot, mYaw, mPitch, mRoll;
			//おそらくボーンの初期の向きが正位置になっているはずです.
			D3DXMatrixTranslation(&mDevia, vSpecifiedPos.x, vSpecifiedPos.y, vSpecifiedPos.z);//ずらしたい方向の行列を作成.
			D3DXMatrixMultiply(&mtmp, &mDevia, &mtmp);//ボーン位置行列とずらしたい方向行列を掛け合わせる.
			D3DXVECTOR3 tmpPos = D3DXVECTOR3(mtmp._41, mtmp._42, mtmp._43);//位置のみ取得.

			D3DXMatrixScaling(&mScale, m_Tranceform.Scale.x, m_Tranceform.Scale.y, m_Tranceform.Scale.z );
			D3DXMatrixRotationY(&mYaw, m_Tranceform.Rotation.y);
			D3DXMatrixRotationX(&mPitch, m_Tranceform.Rotation.x);
			D3DXMatrixRotationZ(&mRoll, m_Tranceform.Rotation.z);
			D3DXMatrixTranslation(&mTran, tmpPos.x, tmpPos.y, tmpPos.z);

			mRot = mYaw * mPitch * mRoll;
			mWorld = mTran * mScale * mRot;

			pOutPos->x = mWorld._41 + m_Tranceform.Position.x;
			pOutPos->y = mWorld._42 + m_Tranceform.Position.y;
			pOutPos->z = mWorld._43 + m_Tranceform.Position.z;

			return true;
		}
	}
	return false;
}

//コンスタントバッファ作成関数.
HRESULT CDX9SkinMesh::CreateCBuffer(
	ID3D11Buffer** pConstantBuffer,
	UINT size)
{
	D3D11_BUFFER_DESC cb;

	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = size;
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;
	if (FAILED(
		m_pDevice11->CreateBuffer(&cb, NULL, pConstantBuffer)))
	{
		return E_FAIL;
	}
	return S_OK;
}

//サンプラー作成関数.
HRESULT CDX9SkinMesh::CreateLinearSampler(ID3D11SamplerState** pSampler)
{
	//テクスチャー用サンプラー作成.
	D3D11_SAMPLER_DESC SamDesc;
	ZeroMemory(&SamDesc, sizeof(D3D11_SAMPLER_DESC));

	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	if (FAILED(
		m_pDevice11->CreateSamplerState(&SamDesc, &m_pSampleLinear)))
	{
		return E_FAIL;
	}
	return S_OK;
}