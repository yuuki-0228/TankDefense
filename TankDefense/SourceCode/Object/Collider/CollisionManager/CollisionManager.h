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

namespace coll
{

	// 球体同士の当たり判定.
	bool IsSphereToSphere( CSphere* pMySphere, CSphere* pOppSphere );

	// カプセル同士の当たり判定.
	bool IsCapsuleToCapsule( CCapsule* pMyCapsule, CCapsule* pOppCapsule );

	// ボックス同士の当たり判定.
	bool IsOBBToOBB( CBox* pMyBox, CBox* pOppBox );

	// レイと球体の当たり判定.
	bool IsRayToSphere( CRay* pRay, CSphere* pSphere, D3DXVECTOR3* pOutStartPos, D3DXVECTOR3* pOutEndPos );

};

#endif	// #ifndef COLLISION_MANAGER_H.