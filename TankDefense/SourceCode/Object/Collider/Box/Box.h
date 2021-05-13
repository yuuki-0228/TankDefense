/**
* @file Box.h.
* @brief ボックスクラス.
* @author 福田玲也.
*/
#ifndef BOX_H
#define BOX_H

#include "..\ColliderBase.h"

/*****************************************
*	ボックスクラス.
**/
class CBox : public CColliderBase
{
public:
	CBox();
	virtual ~CBox();

	// 方向ベクトルの取得.
	D3DXVECTOR3 GetDirection( const int& index );
};

#endif	// #ifndef BOX_H.