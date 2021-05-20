/**
* @file Colliders.h.
* @brief �����蔻����܂Ƃ߂��N���X.
* @author ���c���.
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

	Sphere,		// ����.
	Capsule,	// �J�v�Z��.
	Box,		// �{�b�N�X.
	Ray,		// ���C.
	Mesh,		// ���b�V��.

	Max,
} typedef ECollNo;


/******************************************
*	�R���W�����Y�N���X.
*	 �e�R���W�������܂Ƃ߂��N���X.
*	 ���̂悤�ɓ����蔻��p�̐}�`��ݒ肵�Ď擾����.
*	 GetCollision<CSphere>();
**/
class CCollisions
{
public:
	CCollisions();
	~CCollisions();

	// �w�肵�������蔻��̏�����.
	void InitCollision( const ECollNo& no, const STranceform& t = STranceform() );

	// �����蔻��̎擾 �߂�l�̃I�[�o�[�I�[�h�悤�Ɏg�p.
	template<class T> T*	GetCollision(){ return nullptr; }
	// �����蔻��̎擾(����).
	template<> CSphere*		GetCollision(){ return m_pSphere.get(); }
	// �����蔻��̎擾(����).
	template<> CCapsule*	GetCollision(){ return m_pCapsule.get(); }
	// �����蔻��̎擾(�{�b�N�X).
	template<> CBox*		GetCollision(){ return m_pBox.get(); }
	// �����蔻��̎擾(���C).
	template<> CRay*		GetCollision(){ return m_pRay.get(); }
	// �����蔻��̎擾(���C).
	template<> CMesh*		GetCollision(){ return m_pMesh.get(); }

private:
	std::unique_ptr<CSphere>	m_pSphere;	// ����.
	std::unique_ptr<CCapsule>	m_pCapsule;	// �J�v�Z��.
	std::unique_ptr<CBox>		m_pBox;		// �{�b�N�X.
	std::unique_ptr<CRay>		m_pRay;		// ���C.
	std::unique_ptr<CMesh>		m_pMesh;	// ���C.
};

#endif	// #ifndef COLLISIONS_H.