/**
* @file Actor.h.
* @brief ゲーム内のキャラクターや、弾などの基底クラス.
* @author 福田玲也.
*/
#ifndef ACTOR_H
#define ACTOR_H

#include "..\GameObject.h"
#include "..\..\Collider\CollisionManager\CollisionManager.h"
#include "..\..\Collider\Colliders\Colliders.h"

/******************************************
*	アクタークラス.
*	 互いに干渉するようなキャラクターや、
*	 弾などに継承させるクラス.
**/
class CActor : public CGameObject
{
public:
	CActor();
	virtual ~CActor();

	// 当たり判定関数.
	virtual void Collision( CActor* pActor ) = 0;

	// 当たり判定の初期化.
	virtual void InitCollision() = 0;
	// 当たり判定の座標や、半径などの更新.
	//	Update関数の最後に呼ぶ.
	virtual void UpdateCollision() = 0;


	// 当たり判定の取得 戻り値のオーバーオードように使用.
	template<class T> T*	GetCollision(){ return nullptr; }
	// 当たり判定の取得(球体).
	template<> CSphere*		GetCollision(){ return m_pCollisions->GetCollision<CSphere>(); }
	// 当たり判定の取得(球体).
	template<> CCapsule*	GetCollision(){ return m_pCollisions->GetCollision<CCapsule>(); }
	// 当たり判定の取得(ボックス).
	template<> CBox*		GetCollision(){ return m_pCollisions->GetCollision<CBox>(); }
	// 当たり判定の取得(レイ).
	template<> CRay*		GetCollision(){ return m_pCollisions->GetCollision<CRay>(); }

private:
	std::unique_ptr<CCollisions>	m_pCollisions;	// 複数のコリジョンを所持しているクラス.
};

#endif	// #ifndef ACTOR_H.