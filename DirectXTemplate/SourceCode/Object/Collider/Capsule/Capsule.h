/**
* @file Capsule.h.
* @brief カプセルクラス.
* @author 福田玲也.
*/
#ifndef CAPSULE_H
#define CAPSULE_H

#include "..\ColliderBase.h"
#include "CapsuleCalculation.h"

/*****************************************
*	カプセルクラス.
**/
class CCapsule : public CColliderBase
{
public:
	CCapsule();
	virtual ~CCapsule();

	// 半径の設定.
	inline void		SetRadius( const float& rad )	{ m_Radius = rad; }
	// 半径の取得.
	inline float	GetRadius() const				{ return m_Radius; }
	// 高さの設定.
	inline void		SetHeight( const float& height ){ m_Height = height; }
	// 高さの取得.
	inline float	GetHeight() const				{ return m_Height; }

	SSegment GetSegment();

private:
	float m_Radius;	// 半径.
	float m_Height;	// カプセルの高さ.
};

#endif	// #ifndef SPHERE_H.