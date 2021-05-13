#ifndef NORMAL_LIGHT_H
#define NORMAL_LIGHT_H

#include "..\LightBase.h"

/******************************************
*	 通常ライト.
**/
class CNormalLight : public CLightBase
{
public:
	CNormalLight();
	virtual ~CNormalLight();

	// 更新関数.
	virtual void Update( const float& deltaTime ) override;
};

#endif	// #ifndef NORMAL_LIGHT_H.