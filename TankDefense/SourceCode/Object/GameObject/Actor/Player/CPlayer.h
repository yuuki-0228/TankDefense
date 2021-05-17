#pragma once
#include "..\Actor.h"
#include "..\..\..\..\Common\Mesh\RayMesh\RayMesh.h"

class CDX9SkinMesh;
class CRotLookAtCenter;

class CPlayer
	: public CActor
{
public:
	CPlayer();
	~CPlayer();


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

protected:
	CDX9SkinMesh*	m_pSkinMesh;
	std::unique_ptr<CRotLookAtCenter>	m_pLookCamera;

	D3DXVECTOR3		m_pos;
	D3DXVECTOR3		m_Rot;
	D3DXVECTOR3		m_MoveVec3;
};