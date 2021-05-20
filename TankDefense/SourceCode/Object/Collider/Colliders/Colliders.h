/**
* @file Colliders.h.
* @brief 当たり判定をまとめたクラス.
* @author 福田玲也.
*/
#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "..\Sphere\Sphere.h"
#include "..\Capsule\Capsule.h"
#include "..\Box\Box.h"
#include "..\Ray\Ray.h"
#include "..\Mesh\Mesh.h"

#include <memory>

enum class enCollNo : unsigned int
{
	None,

	Sphere,		// 球体.
	Capsule,	// カプセル.
	Box,		// ボックス.
	Ray,		// レイ.
	Mesh,		// メッシュ.

	Max,
} typedef ECollNo;


/******************************************
*	コリジョンズクラス.
*	 各コリジョンをまとめたクラス.
*	 ↓のように当たり判定用の図形を設定して取得する.
*	 GetCollision<CSphere>();
**/
class CCollisions
{
public:
	CCollisions();
	~CCollisions();

	// 指定した当たり判定の初期化.
	void InitCollision( const ECollNo& no, const STranceform& t = STranceform() );

	// 当たり判定の取得 戻り値のオーバーオードように使用.
	template<class T> T*	GetCollision(){ return nullptr; }
	// 当たり判定の取得(球体).
	template<> CSphere*		GetCollision(){ return m_pSphere.get(); }
	// 当たり判定の取得(球体).
	template<> CCapsule*	GetCollision(){ return m_pCapsule.get(); }
	// 当たり判定の取得(ボックス).
	template<> CBox*		GetCollision(){ return m_pBox.get(); }
	// 当たり判定の取得(レイ).
	template<> CRay*		GetCollision(){ return m_pRay.get(); }
	// 当たり判定の取得(レイ).
	template<> CMesh*		GetCollision(){ return m_pMesh.get(); }

private:
	std::unique_ptr<CSphere>	m_pSphere;	// 球体.
	std::unique_ptr<CCapsule>	m_pCapsule;	// カプセル.
	std::unique_ptr<CBox>		m_pBox;		// ボックス.
	std::unique_ptr<CRay>		m_pRay;		// レイ.
	std::unique_ptr<CMesh>		m_pMesh;	// レイ.
};

#endif	// #ifndef COLLISIONS_H.