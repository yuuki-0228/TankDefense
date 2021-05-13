/**
* @file ColliderBase.h.
* @brief 当たり判定基底クラス.
* @author 福田玲也.
*/
#ifndef COLLIDER_BASE_H
#define COLLIDER_BASE_H

#include "..\Object.h"

/**********************************************
*	当たり判定用ベースクラス.
**/
class CColliderBase : public CObject
{
public:
	CColliderBase();
	virtual ~CColliderBase();

	// ヒットフラグを立てる.
	inline void SetHitOn()		{ m_IsHit = true; }
	// ヒットフラグを下ろす.
	inline void SetHitOff()		{ m_IsHit = false; }
	inline bool IsHit()	const	{ return m_IsHit; }

protected:
	bool	m_IsHit;	// ヒットフラグ.
};

#endif	// #ifndef COLLIDER_BASE_H.