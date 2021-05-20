/**
* @file Actor.h.
* @brief �Q�[�����̃L�����N�^�[��A�e�Ȃǂ̊��N���X.
* @author ���c���.
*/
#ifndef ACTOR_H
#define ACTOR_H

#include "..\GameObject.h"
#include "..\..\Collider\CollisionManager\CollisionManager.h"
#include "..\..\Collider\Colliders\Colliders.h"

/******************************************
*	�A�N�^�[�N���X.
*	 �݂��Ɋ�����悤�ȃL�����N�^�[��A
*	 �e�ȂǂɌp��������N���X.
**/
class CActor : public CGameObject
{
public:
	CActor();
	virtual ~CActor();

	// �����蔻��֐�.
	virtual void Collision( CActor* pActor ) = 0;

	// �����蔻��̏�����.
	virtual void InitCollision() = 0;
	// �����蔻��̍��W��A���a�Ȃǂ̍X�V.
	//	Update�֐��̍Ō�ɌĂ�.
	virtual void UpdateCollision() = 0;


	// �����蔻��̎擾 �߂�l�̃I�[�o�[�I�[�h�悤�Ɏg�p.
	template<class T> T*	GetCollision(){ return nullptr; }
	// �����蔻��̎擾(����).
	template<> CSphere*		GetCollision(){ return m_pCollisions->GetCollision<CSphere>(); }
	// �����蔻��̎擾(����).
	template<> CCapsule*	GetCollision(){ return m_pCollisions->GetCollision<CCapsule>(); }
	// �����蔻��̎擾(�{�b�N�X).
	template<> CBox*		GetCollision(){ return m_pCollisions->GetCollision<CBox>(); }
	// �����蔻��̎擾(���C).
	template<> CRay*		GetCollision(){ return m_pCollisions->GetCollision<CRay>(); }

protected:
	std::unique_ptr<CCollisions>	m_pCollisions;	// �����̃R���W�������������Ă���N���X.
};

#endif	// #ifndef ACTOR_H.