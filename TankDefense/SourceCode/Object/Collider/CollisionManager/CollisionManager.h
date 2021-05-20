/**
* @file CollisionManager.h.
* @brief �����蔻��N���X.
* @author ���c���.
*/
#ifndef COLLISION_MANAGER_H
#define COLLISION_MANAGER_H

// �x���ɂ��Ă̺��ޕ��͂𖳌��ɂ���.4005:�Ē�`.
#pragma warning(disable:4005)

#include <D3DX11.h>
#include <D3DX10.h>	//�uD3DX�`�v�̒�`�g�p���ɕK�v.
#include <D3D10.h>

// ���C�u�����ǂݍ���.
#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "d3dx11.lib" )
#pragma comment( lib, "d3dx10.lib" )	//�uD3DX�`�v�̒�`�g�p���ɕK�v.

class CSphere;
class CCapsule;
class CBox;
class CRay;
class CMesh;

namespace coll
{
	/*
	*	���̓��m�̓����蔻��.
	*	@param[in] pMySphere	�����̋��̃N���X.
	*	@param[in] pOppSphere	����̋��̃N���X.
	*	@return �Փ˂����� true ���ĂȂ��ꍇ�� false.
	**/
	bool IsSphereToSphere( CSphere* pMySphere, CSphere* pOppSphere );

	/*
	*	�J�v�Z�����m�̓����蔻��.
	*	@param[in] pMyCapsule	�����̃J�v�Z���N���X.
	*	@param[in] pOppCapsule	����̃J�v�Z���N���X.
	*	@return �Փ˂����� true ���ĂȂ��ꍇ�� false.
	**/
	bool IsCapsuleToCapsule( CCapsule* pMyCapsule, CCapsule* pOppCapsule );

	/*
	*	�{�b�N�X���m�̓����蔻��.
	*	@param[in] pMyBox	�����̃{�b�N�X�N���X.
	*	@param[in] pOppBox	����̃{�b�N�X�N���X.
	*	@return �Փ˂����� true ���ĂȂ��ꍇ�� false.
	**/
	bool IsOBBToOBB( CBox* pMyBox, CBox* pOppBox );

	/*
	*	���C�Ƌ��̂̓����蔻��.
	*	@param[in]	pRay			���C�X�N���X.
	*	@param[in]	pSphere			���̃N���X.
	*	@param[out]	pOutStartPos	���C���Փ˂��Ă���ꍇ�A�Փ˂����J�n���W��Ԃ�.
	*	@param[out]	pOutEndPos		���C���Փ˂��Ă���ꍇ�A�Փ˂����I�����W��Ԃ�.
	*	@return �Փ˂����� true ���ĂȂ��ꍇ�� false.
	**/
	bool IsRayToSphere( CRay* pRay, CSphere* pSphere, D3DXVECTOR3* pOutStartPos = nullptr, D3DXVECTOR3* pOutEndPos = nullptr );

	/*
	*	���C�ƃ��b�V���̓����蔻��.
	*	@param[in]	pRay			���C�X�N���X.
	*	@param[in]	pMesh			���b�V���N���X.
	*	@param[out]	pOutDistance	���C�ƃ��b�V���̋�����Ԃ�.
	*	@param[out]	pIntersect		�Փ˂��Ă���ꍇ���C�ƃ|���S��(���b�V��)�Ƃ̌������W��Ԃ�.
	*	@param[out]	pOutNormal		�Փ˂��Ă���ꍇ�|���S���̖@����Ԃ�.
	*	@param[in]	isNormalHit		�|���S���̖@���Ɣ�r���Ĕ��肷�邩�B false�̏ꍇ�|���S��������ł����肷��.
	*	@return �Փ˂����� true ���ĂȂ��ꍇ�� false.
	**/
	bool IsRayToMesh( CRay* pRay, CMesh* pMesh, float* pOutDistance, D3DXVECTOR3* pIntersect, D3DXVECTOR3* pOutNormal = nullptr, const bool& isNormalHit = true );

};

#endif	// #ifndef COLLISION_MANAGER_H.