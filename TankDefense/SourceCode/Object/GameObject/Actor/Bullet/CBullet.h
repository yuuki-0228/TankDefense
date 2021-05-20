#pragma once
#include "..\Actor.h"
#include "..\..\..\..\Common\Mesh\RayMesh\RayMesh.h"

class CDX9StaticMesh;

class CBullet
	: public CActor
{
public:
	CBullet();
	~CBullet();

	// 初期化関数.
	virtual bool Init();
	// 更新関数.
	virtual void Update( const float& deltaTime );
	// 描画関数.
	virtual void Render();

	// 当たり判定関数.
	virtual void Collision( CActor* pActor );

	// 当たり判定の初期化.
	virtual void InitCollision();
	// 当たり判定の座標や、半径などの更新.
	//	Update関数の最後に呼ぶ.
	virtual void UpdateCollision();

	void Shot( D3DXVECTOR3 Pos, D3DXVECTOR3 MoveVec );

	bool GetShotFalg() const { return m_ShotFlag; }

private:
	CDX9StaticMesh*	m_pStaticMesh;

	D3DXVECTOR3		m_pos;
	D3DXVECTOR3		m_MoveVec3;

	bool			m_ShotFlag;
};