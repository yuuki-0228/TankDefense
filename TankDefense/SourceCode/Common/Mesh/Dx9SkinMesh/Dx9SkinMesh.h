/***************************************************************************************************
*	SkinMeshCode Version 2.00
*	LastUpdate	: 2019/10/09
**/
#ifndef DX9_SKIN_MESH_H
#define DX9_SKIN_MESH_H

#include "Parser/CDX9SkinMeshParser.h"
#include "..\..\Common.h"
#include "..\MeshStruct.h"

struct stAnimationController
{
	LPD3DXANIMATIONCONTROLLER	pAC;
	double						AnimTime;
	bool						IsAnimChangeBlend;

	stAnimationController()
		: pAC				( nullptr )
		, AnimTime			( 0.0 )
		, IsAnimChangeBlend	( false )
	{}

	void BlendAnimUpdate( const double& speed )
	{
		// アニメーション切り替えフラグが下りてたら終了.
		if( IsAnimChangeBlend == false ) return;
		AnimTime += speed;

		float weight = static_cast<float>(AnimTime) / 1.0f;	// ウェイトの計算.
		pAC->SetTrackWeight( 0, weight );	// トラック0 にウェイトを設定.
		pAC->SetTrackWeight( 1, 1.0f - weight );// トラック1 にウェイトを設定.

		if( AnimTime < 1.0 ) return;
		// アニメーションタイムが一定値に達したら.
		IsAnimChangeBlend = false;	// フラグを下す.
		pAC->SetTrackWeight( 0, 1.0f );		// ウェイトを1に固定する.
		pAC->SetTrackEnable( 1, false );	// トラック1を無効にする.
	}

	HRESULT SetAnimController( LPD3DXANIMATIONCONTROLLER pAc )
	{
		if( FAILED( pAc->CloneAnimationController(
			pAc->GetMaxNumAnimationOutputs(),
			pAc->GetMaxNumAnimationSets(),
			pAc->GetMaxNumTracks(),
			pAc->GetMaxNumEvents(),
			&pAC ))){
			return E_FAIL;
		}
		return S_OK;
	}

} typedef SAnimationController;

//==================================================================================================
//
//	スキンメッシュクラス.
//
//==================================================================================================
class CDX9SkinMesh : public CCommon
{
public:
	//ボーン単位.
	struct CBUFFER_PER_BONES
	{
		D3DXMATRIX mBone[D3DXPARSER::MAX_BONES];
		CBUFFER_PER_BONES()
		{
			for (int i = 0; i < D3DXPARSER::MAX_BONES; i++)
			{
				D3DXMatrixIdentity(&mBone[i]);
			}
		}
	};


	//頂点構造体.
	struct MY_SKINVERTEX
	{
		D3DXVECTOR3	vPos;		//頂点位置.
		D3DXVECTOR3	vNorm;		//頂点法線.
		D3DXVECTOR2	vTex;		//UV座標.
		D3DXVECTOR3	Tangent;	// .
		D3DXVECTOR3	Binormal;	// 法線(陰影計算に必須).
		UINT bBoneIndex[4];		//ボーン 番号.
		float bBoneWeight[4];	//ボーン 重み.
		MY_SKINVERTEX()
			: vPos()
			, vNorm()
			, vTex()
			, bBoneIndex()
			, bBoneWeight()
		{}
	};

	//パーサークラスからアニメーションコントローラーを取得する.
	LPD3DXANIMATIONCONTROLLER GetAnimController()
	{
		return m_pD3dxMesh->m_pAnimController;
	}

	//メソッド.
	CDX9SkinMesh( 
		HWND hWnd, 
		ID3D11Device* pDevice11,
		ID3D11DeviceContext* pContext11, 
		LPDIRECT3DDEVICE9 pDevice9,
		const char* fileName )
		: CDX9SkinMesh()
	{
		Init( hWnd, pDevice11, pContext11, pDevice9, fileName );
	}
	CDX9SkinMesh();
	~CDX9SkinMesh();

	HRESULT Init(
		HWND hWnd, 
		ID3D11Device* pDevice11,
		ID3D11DeviceContext* pContext11,
		LPDIRECT3DDEVICE9 pDevice9,
		const char* fileName );

	//Xファイルからスキンメッシュを作成する.
	HRESULT LoadXMesh( const char* fileName );
	//描画関数.
	void Render( SAnimationController* pAC=NULL );
	//解放関数.
	HRESULT Release();

	double GetAnimSpeed()				{ return m_dAnimSpeed;		}
	void SetAnimSpeed( double dSpeed )	{ m_dAnimSpeed = dSpeed;	}

	double GetAnimTime()				{ return m_dAnimTime;		}
	void SetAnimTime( double dTime )	{ m_dAnimTime = dTime;		}

	//アニメーションコントローラを取得.
	LPD3DXANIMATIONCONTROLLER GetAnimationController() { return m_pD3dxMesh->m_pAnimController; }

	//アニメーションセットの切り替え.
	void ChangeAnimSet( int index, SAnimationController* pAC=nullptr );
	//アニメーションセットの切り替え(開始フレーム指定可能版).
	void ChangeAnimSet_StartPos( int index, double dStartFramePos, SAnimationController* pAC=nullptr );
	// アニメーションをブレンドして切り替え.
	void ChangeAnimBlend( int index, int oldIndex, SAnimationController* pAC = nullptr );

	// ブレンドアニメーションの更新.
	void BlendAnimUpdate();

	//アニメーション停止時間を取得.
	double GetAnimPeriod( int index );
	//アニメーション数を取得.
	int GetAnimMax( LPD3DXANIMATIONCONTROLLER pAC=NULL );

	//指定したボーン情報(座標・行列)を取得する関数.
	bool GetMatrixFromBone(const char* sBoneName, D3DXMATRIX* pOutMat );
	bool GetPosFromBone(const char* sBoneName, D3DXVECTOR3* pOutPos);
	bool SetMatrixFromBone(const char* sBoneName, D3DXMATRIX inMat );
	bool SetPosFromBone(const char* sBoneName, D3DXVECTOR3 inPos );
	bool GetDeviaPosFromBone(const char* sBoneName, D3DXVECTOR3* pOutPos, D3DXVECTOR3 vSpecifiedPos = { 0.0f, 0.0f, 0.0f });

	//ﾒｯｼｭを取得.
	LPD3DXMESH GetMesh() const { return m_pMeshForRay; }

private:
	HWND m_hWnd;

	//Dx9.
	LPDIRECT3DDEVICE9		m_pDevice9;

	//Dx11.
	ID3D11SamplerState*		m_pSampleLinear;
	ID3D11VertexShader*		m_pVertexShader;
	ID3D11PixelShader*		m_pPixelShader;
	ID3D11InputLayout*		m_pVertexLayout;
	ID3D11Buffer*			m_pCBufferPerMesh;		//コンスタントバッファ(メッシュ毎).
	ID3D11Buffer*			m_pCBufferPerMaterial;	//コンスタントバッファ(マテリアル毎).
	ID3D11Buffer*			m_pCBufferPerFrame;		//コンスタントバッファ(フレーム毎).
	ID3D11Buffer*			m_pCBufferPerBone;		//コンスタントバッファ(ボーン毎).

	D3DXMATRIX		m_mWorld;
	D3DXMATRIX		m_mRotation;

	D3DXMATRIX		m_mView;
	D3DXMATRIX		m_mProj;

	D3DXVECTOR3		m_CameraPos;
	D3DXVECTOR3		m_CameraLookPos;

	//アニメーション速度.
	double m_dAnimSpeed;
	double m_dAnimTime;

	bool m_IsChangeAnim;

	//解放処理用に.
	SKIN_PARTS_MESH* m_pReleaseMaterial;

	//メッシュ.
	D3DXPARSER* m_pD3dxMesh;
	LPD3DXMESH	m_pMeshForRay;	//ﾚｲとﾒｯｼｭ用.

	//XFileのパス.
	CHAR	m_FilePath[256];

	//アニメーションフレーム.
	int		m_iFrame;

	HRESULT InitShader();
	HRESULT CreateIndexBuffer( DWORD dwSize, int* pIndex, ID3D11Buffer** ppIndexBuffer );
	void RecursiveSetNewPoseMatrices( BONE* pBone,D3DXMATRIX* pmParent );

	//全てのメッシュを作成する.
	void BuildAllMesh( D3DXFRAME* pFrame );
	
	//メッシュを作成する.
	HRESULT CreateAppMeshFromD3DXMesh( LPD3DXFRAME pFrame );

	//Xファイルからスキン関連の情報を読み出す関数.
	HRESULT ReadSkinInfo( MYMESHCONTAINER* pContainer, MY_SKINVERTEX* pvVB, SKIN_PARTS_MESH* pParts );

	//ボーンを次のポーズ位置にセットする関数.
	void SetNewPoseMatrices( SKIN_PARTS_MESH* pParts, int frame, MYMESHCONTAINER* pContainer );
	//次の(現在の)ポーズ行列を返す関数.
	D3DXMATRIX GetCurrentPoseMatrix( SKIN_PARTS_MESH* pParts, int index );

	//フレーム描画.
	void DrawFrame( LPD3DXFRAME pFrame );
	//パーツ描画.
	void DrawPartsMesh( SKIN_PARTS_MESH* p, D3DXMATRIX World, MYMESHCONTAINER* pContainer );
	void DrawPartsMeshStatic( SKIN_PARTS_MESH* pMesh, D3DXMATRIX World, MYMESHCONTAINER* pContainer );

	//全てのメッシュを削除.
	void DestroyAllMesh( D3DXFRAME* pFrame );
	HRESULT DestroyAppMeshFromD3DXMesh( LPD3DXFRAME p );

	//コンスタントバッファ作成関数.
	HRESULT CreateCBuffer(ID3D11Buffer** pConstantBuffer, UINT size);
	//サンプラー作成関数.
	HRESULT CreateLinearSampler(ID3D11SamplerState** pSampler);
};

#endif//#ifndef DX9_SKIN_MESH_H.