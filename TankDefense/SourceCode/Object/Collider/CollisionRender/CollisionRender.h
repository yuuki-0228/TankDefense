/**
* @file CollisionRender.h.
* @brief 当たり判定描画用クラス.
* @author 福田玲也.
*/
#ifndef COLLISION_RENDER_H
#define COLLISION_RENDER_H

#include <queue>
#include <memory>

class CDX9StaticMesh;
class CSphere;
class CCapsule;
class CBox;
class CRay;
class CRayMesh;

/*************************************
*	当たり判定の描画用クラス.
**/
class CCollisionRender
{
public:
	CCollisionRender();
	~CCollisionRender();

	// インスタンスの取得.
	static CCollisionRender* GetInstance();

	// 描画.
	static void Render();

	// 球体の追加.
	static void PushSphere( CSphere* pSphere );
	// カプセルの追加.
	static void PushCapsule( CCapsule* pCapsule );
	// ボックスの追加.
	static void PushBox( CBox* pBox );
	// レイの追加.
	static void PushRay( CRay* pBox );

private:
	// 球体の描画.
	void SphereRender();
	// カプセルの描画.
	void CapsuleRender();
	// ボックスの描画.
	void BoxRender();
	// レイの描画.
	void RayRender();

private:
	std::queue<CSphere*>		m_pSphereQueue;	// 球体のキュー.
	std::queue<CCapsule*>		m_pCapsuleQueue;// カプセルのキュー.
	std::queue<CBox*>			m_pBoxQueue;	// ボックスのキュー.
	std::queue<CRay*>			m_pRayQueue;	// レイのキュー.
	CDX9StaticMesh*				m_pSphere;		// 球体メッシュ.
	CDX9StaticMesh*				m_pCapsule;		// カプセルメッシュ.
	CDX9StaticMesh*				m_pBox;			// ボックスメッシュ.
	std::unique_ptr<CRayMesh>	m_pRay;			// レイメッシュ.

	bool					m_IsSphreRender;	// 球体を描画するか.
	bool					m_IsCapsuleRender;	// カプセルを描画するか.
	bool					m_IsBoxRender;		// ボックスを描画するか.
	bool					m_IsRayRender;		// レイを描画するか.

private:
	// コピー・ムーブコンストラクタ, 代入演算子の削除.
	CCollisionRender( const CCollisionRender & )				= delete;
	CCollisionRender& operator = ( const CCollisionRender & )	= delete;
	CCollisionRender( CCollisionRender && )						= delete;
	CCollisionRender& operator = ( CCollisionRender && )		= delete;
};

#endif	// #ifndef COLLISION_RENDER_H.