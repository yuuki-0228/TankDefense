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

	// �������֐�.
	virtual bool Init();
	// �X�V�֐�.
	virtual void Update( const float& deltaTime );
	// �`��֐�.
	virtual void Render();

	// �����蔻��֐�.
	virtual void Collision( CActor* pActor );

	// �����蔻��̏�����.
	virtual void InitCollision();
	// �����蔻��̍��W��A���a�Ȃǂ̍X�V.
	//	Update�֐��̍Ō�ɌĂ�.
	virtual void UpdateCollision();

	void Shot( D3DXVECTOR3 Pos, D3DXVECTOR3 MoveVec );

	bool GetShotFalg() const { return m_ShotFlag; }

private:
	CDX9StaticMesh*	m_pStaticMesh;

	D3DXVECTOR3		m_pos;
	D3DXVECTOR3		m_MoveVec3;

	bool			m_ShotFlag;
};