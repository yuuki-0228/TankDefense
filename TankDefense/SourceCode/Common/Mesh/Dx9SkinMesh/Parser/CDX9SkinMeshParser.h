/***************************************************************************************************
*	SkinMeshCode Version 2.00
*	LastUpdate	: 2019/10/09
**/
#ifndef C_DX9SKIN_MESH_PARSER_H
#define C_DX9SKIN_MESH_PARSER_H
//警告についてのコード分析を無効にする。4005：再定義.
#pragma warning( disable : 4005 )

#include <d3dx9.h>

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

#include "..\..\..\..\Global.h"


//オリジナル　マテリアル構造体.
struct MY_SKINMATERIAL
{
	char Name[110];
	D3DXVECTOR4	Ka;	//アンビエント.
	D3DXVECTOR4	Kd;	//ディフューズ.
	D3DXVECTOR4	Ks;	//スペキュラー.
	char TextureName[512];	//テクスチャーファイル名.
	ID3D11ShaderResourceView* pTexture;
	ID3D11ShaderResourceView* pNormTexture;
	DWORD dwNumFace;	//そのマテリアルであるポリゴン数.
	MY_SKINMATERIAL()
		: Name()
		, Ka()
		, Kd()
		, Ks()
		, TextureName()
		, pTexture(nullptr)
		, pNormTexture(nullptr)
		, dwNumFace()
	{
		ZeroMemory(this, sizeof(MY_SKINMATERIAL));
	}
	~MY_SKINMATERIAL()
	{
		if (pTexture != nullptr)
		{
			pTexture->Release();
			pTexture = nullptr;
		}
		if (pNormTexture != nullptr)
		{
			pNormTexture->Release();
			pNormTexture = nullptr;
		}
	}
};

//ボーン構造体.
struct BONE
{
	D3DXMATRIX mBindPose;	//初期ポーズ（ずっと変わらない）.
	D3DXMATRIX mNewPose;	//現在のポーズ（その都度変わる）.
	DWORD dwNumChild;		//子の数.
	int iChildIndex[50];	//自分の子の“インデックス”50個まで.
	char Name[256];

	BONE()
		: mBindPose()
		, mNewPose()
		, dwNumChild()
		, iChildIndex()
		, Name()
	{
		D3DXMatrixIdentity(&mBindPose);
		D3DXMatrixIdentity(&mNewPose);
	}
};

//パーツメッシュ構造体.
struct SKIN_PARTS_MESH
{
	DWORD	dwNumVert;
	DWORD	dwNumFace;
	DWORD	dwNumUV;
	DWORD				dwNumMaterial;
	MY_SKINMATERIAL*	pMaterial;
	char				TextureFileName[8][256];	//テクスチャーファイル名(8枚まで).
	bool				bTex;

	ID3D11Buffer*	pVertexBuffer;
	ID3D11Buffer**	ppIndexBuffer;

	//ボーン.
	int		iNumBone;
	BONE*	pBoneArray;

	bool	bEnableBones;	//ボーンの有無フラグ.

	SKIN_PARTS_MESH()
		: dwNumVert()
		, dwNumFace()
		, dwNumUV()
		, dwNumMaterial()
		, pMaterial(nullptr)
		, TextureFileName()
		, bTex()
		, pVertexBuffer(nullptr)
		, ppIndexBuffer(nullptr)
		, iNumBone()
		, pBoneArray(nullptr)
		, bEnableBones()
	{}
};

//派生フレーム構造体.
//	それぞれのメッシュ用の最終ワールド行列を追加する.
struct MYFRAME : public D3DXFRAME
{
	D3DXMATRIX CombinedTransformationMatrix;
	SKIN_PARTS_MESH* pPartsMesh;
	MYFRAME()
		: D3DXFRAME()
		, CombinedTransformationMatrix()
		, pPartsMesh(nullptr)
	{
	}
};
//派生メッシュコンテナー構造体.
//	コンテナーがテクスチャを複数持てるようにポインターのポインターを追加する
struct MYMESHCONTAINER : public D3DXMESHCONTAINER
{
	LPDIRECT3DTEXTURE9*  ppTextures;
	DWORD dwWeight;				//重みの個数（重みとは頂点への影響。）.
	DWORD dwBoneNum;			//ボーンの数.
	LPD3DXBUFFER pBoneBuffer;	//ボーン・テーブル.
	D3DXMATRIX** ppBoneMatrix;	//全てのボーンのワールド行列の先頭ポインタ.
	D3DXMATRIX* pBoneOffsetMatrices;//フレームとしてのボーンのワールド行列のポインタ.
	MYMESHCONTAINER()
		: D3DXMESHCONTAINER()
		, ppTextures(nullptr)
		, dwWeight()
		, dwBoneNum()
		, pBoneBuffer(nullptr)
		, ppBoneMatrix(nullptr)
		, pBoneOffsetMatrices(nullptr)
	{}
};
//Xファイル内のアニメーション階層を読み下してくれるクラスを派生させる.
//	ID3DXAllocateHierarchyは派生すること想定して設計されている.
class MY_HIERARCHY : public ID3DXAllocateHierarchy
{
public:
	MY_HIERARCHY()
		: ID3DXAllocateHierarchy(){}
	STDMETHOD(CreateFrame)(THIS_ LPCSTR, LPD3DXFRAME *);
	STDMETHOD(CreateMeshContainer)(THIS_ LPCSTR, CONST D3DXMESHDATA*, CONST D3DXMATERIAL*,
		CONST D3DXEFFECTINSTANCE*, DWORD, CONST DWORD *, LPD3DXSKININFO, LPD3DXMESHCONTAINER *);
	STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME);
	STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER);
};

//==================================================================================================
//
//	パーサークラス.
//
//==================================================================================================
class D3DXPARSER
{
public:
	//最大ボーン数.
	static const int MAX_BONES = 255;
	//最大アニメーションセット数.
	static const int MAX_ANIM_SET = 100;


	MY_HIERARCHY cHierarchy;
	MY_HIERARCHY* m_pHierarchy;
	LPD3DXFRAME m_pFrameRoot;

	LPD3DXANIMATIONCONTROLLER m_pAnimController;	//デフォルトで一つ.
	LPD3DXANIMATIONSET m_pAnimSet[MAX_ANIM_SET];

	D3DXPARSER();
	~D3DXPARSER();

	HRESULT LoadMeshFromX(LPDIRECT3DDEVICE9, const char* fileName);
	HRESULT AllocateBoneMatrix(LPD3DXMESHCONTAINER);
	HRESULT AllocateAllBoneMatrices(LPD3DXFRAME);
	VOID UpdateFrameMatrices(LPD3DXFRAME, LPD3DXMATRIX);


	int GetNumVertices(MYMESHCONTAINER* pContainer);
	int GetNumFaces(MYMESHCONTAINER* pContainer);
	int GetNumMaterials(MYMESHCONTAINER* pContainer);
	int GetNumUVs(MYMESHCONTAINER* pContainer);
	int GetNumBones(MYMESHCONTAINER* pContainer);
	int GetNumBoneVertices(MYMESHCONTAINER* pContainer, int iBoneIndex);
	DWORD GetBoneVerticesIndices(MYMESHCONTAINER* pContainer, int iBoneIndex, int iIndexInGroup);
	double GetBoneVerticesWeights(MYMESHCONTAINER* pContainer, int iBoneIndex, int iIndexInGroup);
	D3DXVECTOR3 GetVertexCoord(MYMESHCONTAINER* pContainer, DWORD iIndex);
	D3DXVECTOR3 GetNormal(MYMESHCONTAINER* pContainer, DWORD iIndex);
	D3DXVECTOR2 GetUV(MYMESHCONTAINER* pContainer, DWORD iIndex);
	D3DXVECTOR3 GetTangent(MYMESHCONTAINER* pContainer, DWORD iIndex);
	D3DXVECTOR3 GetBinormal(MYMESHCONTAINER* pContainer, DWORD iIndex);
	int GetIndex(MYMESHCONTAINER* pContainer, DWORD iIndex);
	D3DXVECTOR4 GetAmbient(MYMESHCONTAINER* pContainer, int iIndex);
	D3DXVECTOR4 GetDiffuse(MYMESHCONTAINER* pContainer, int iIndex);
	D3DXVECTOR4 GetSpecular(MYMESHCONTAINER* pContainer, int iIndex);
	CHAR* GetTexturePath(MYMESHCONTAINER* pContainer, int index);
	float GetSpecularPower(MYMESHCONTAINER* pContainer, int iIndex);
	int GeFaceMaterialIndex(MYMESHCONTAINER* pContainer, int iFaceIndex);
	int GetFaceVertexIndex(MYMESHCONTAINER* pContainer, int iFaceIndex, int iIndexInFace);
	D3DXMATRIX GetBindPose(MYMESHCONTAINER* pContainer, int iBoneIndex);
	D3DXMATRIX GetNewPose(MYMESHCONTAINER* pContainer, int iBoneIndex);
	CHAR* GetBoneName(MYMESHCONTAINER* pContainer, int iBoneIndex);

	//メッシュコンテナを取得する.
	LPD3DXMESHCONTAINER GetMeshContainer(LPD3DXFRAME pFrame);

	//アニメーションセットの切り替え.
	void ChangeAnimSet(int index, LPD3DXANIMATIONCONTROLLER pAC = nullptr);
	//アニメーションセットの切り替え(開始フレーム指定可能版).
	void ChangeAnimSet_StartPos(int index, double dStartFramePos, LPD3DXANIMATIONCONTROLLER pAC = nullptr);
	// アニメーションをブレンドして切り替え.
	void ChangeAnimBlend( int index, int oldIndex, LPD3DXANIMATIONCONTROLLER pAC = nullptr );

	//アニメーション停止時間を取得.
	double GetAnimPeriod(int index);
	//アニメーション数を取得.
	int GetAnimMax(LPD3DXANIMATIONCONTROLLER pAC = nullptr);

	//指定したボーン情報(座標・行列)を取得する関数.
	bool GetMatrixFromBone(const char* sBoneName, D3DXMATRIX* pOutMat);
	bool GetPosFromBone(const char* sBoneName, D3DXVECTOR3* pOutPos);
	bool SetMatrixFromBone(const char* sBoneName, D3DXMATRIX inMat);
	bool SetPosFromBone(const char* sBoneName, D3DXVECTOR3 inPos);

	//メッシュ解放.
	HRESULT ReleaseMesh(LPD3DXFRAME pFrame);

	//一括解放処理.
	HRESULT Release();
};

#endif//#ifndef C_DX9SKIN_MESH_PARSER_H