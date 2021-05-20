/**
* @file CollisionManager.h.
* @brief 当たり判定クラス.
* @author 福田玲也.
*/
#ifndef COLLISION_MANAGER_H
#define COLLISION_MANAGER_H

// 警告についてのｺｰﾄﾞ分析を無効にする.4005:再定義.
#pragma warning(disable:4005)

#include <D3DX11.h>
#include <D3DX10.h>	//「D3DX～」の定義使用時に必要.
#include <D3D10.h>

// ライブラリ読み込み.
#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "d3dx11.lib" )
#pragma comment( lib, "d3dx10.lib" )	//「D3DX～」の定義使用時に必要.

class CSphere;
class CCapsule;
class CBox;
class CRay;
class CMesh;

namespace coll
{
	/*
	*	球体同士の当たり判定.
	*	@param[in] pMySphere	自分の球体クラス.
	*	@param[in] pOppSphere	相手の球体クラス.
	*	@return 衝突したら true してない場合は false.
	**/
	bool IsSphereToSphere( CSphere* pMySphere, CSphere* pOppSphere );

	/*
	*	カプセル同士の当たり判定.
	*	@param[in] pMyCapsule	自分のカプセルクラス.
	*	@param[in] pOppCapsule	相手のカプセルクラス.
	*	@return 衝突したら true してない場合は false.
	**/
	bool IsCapsuleToCapsule( CCapsule* pMyCapsule, CCapsule* pOppCapsule );

	/*
	*	ボックス同士の当たり判定.
	*	@param[in] pMyBox	自分のボックスクラス.
	*	@param[in] pOppBox	相手のボックスクラス.
	*	@return 衝突したら true してない場合は false.
	**/
	bool IsOBBToOBB( CBox* pMyBox, CBox* pOppBox );

	/*
	*	レイと球体の当たり判定.
	*	@param[in]	pRay			レイスクラス.
	*	@param[in]	pSphere			球体クラス.
	*	@param[out]	pOutStartPos	レイが衝突している場合、衝突した開始座標を返す.
	*	@param[out]	pOutEndPos		レイが衝突している場合、衝突した終了座標を返す.
	*	@return 衝突したら true してない場合は false.
	**/
	bool IsRayToSphere( CRay* pRay, CSphere* pSphere, D3DXVECTOR3* pOutStartPos = nullptr, D3DXVECTOR3* pOutEndPos = nullptr );

	/*
	*	レイとメッシュの当たり判定.
	*	@param[in]	pRay			レイスクラス.
	*	@param[in]	pMesh			メッシュクラス.
	*	@param[out]	pOutDistance	レイとメッシュの距離を返す.
	*	@param[out]	pIntersect		衝突している場合レイとポリゴン(メッシュ)との交差座標を返す.
	*	@param[out]	pOutNormal		衝突している場合ポリゴンの法線を返す.
	*	@param[in]	isNormalHit		ポリゴンの法線と比較して判定するか。 falseの場合ポリゴン裏からでも判定する.
	*	@return 衝突したら true してない場合は false.
	**/
	bool IsRayToMesh( CRay* pRay, CMesh* pMesh, float* pOutDistance, D3DXVECTOR3* pIntersect, D3DXVECTOR3* pOutNormal = nullptr, const bool& isNormalHit = true );

};

#endif	// #ifndef COLLISION_MANAGER_H.