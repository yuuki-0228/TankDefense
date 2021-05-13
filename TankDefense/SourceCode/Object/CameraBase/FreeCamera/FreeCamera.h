#ifndef EDIT_CAMERA_H
#define FREE_CAMERA_H

#include "..\CameraBase.h"
#include <memory>

/****************************************
* フリーカメラ.
**/
class CFreeCamera : public CCameraBase
{
public:
	CFreeCamera();
	virtual ~CFreeCamera();

	// 更新関数.
	virtual void Update( const float& deltaTime ) override;

private:
	// マウスの更新.
	void MouseUpdate();

private:
	D3DXVECTOR2	m_Radian;		// ラジアン.
	float		m_DeltaTime;	// デルタタイム.
};

#endif	// #ifndef EDIT_CAMERA_H.