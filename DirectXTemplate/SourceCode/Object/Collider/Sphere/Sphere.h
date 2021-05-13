/**
* @file Sphere.h.
* @brief ‹…‘ÌƒNƒ‰ƒX.
* @author •Ÿ“c—æ–ç.
*/
#ifndef SPHERE_H
#define SPHERE_H

#include "..\ColliderBase.h"

/*****************************************
*	‹…‘ÌƒNƒ‰ƒX.
**/
class CSphere : public CColliderBase
{
public:
	CSphere();
	virtual ~CSphere();

	// ”¼Œa‚Ìİ’è.
	inline void		SetRadius( const float& rad )	{ m_Radius = rad; }
	// ”¼Œa‚Ìæ“¾.
	inline float	GetRadius() const				{ return m_Radius; }

private:
	float m_Radius;	// ”¼Œa.
};

#endif	// #ifndef SPHERE_H.