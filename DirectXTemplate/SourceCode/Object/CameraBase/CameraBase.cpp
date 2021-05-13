#include "CameraBase.h"

namespace
{
	const float			DEFFULT_ASPECT			= static_cast<float>(WND_W)/static_cast<float>(WND_H);
	const float			MAX_CLIPPING_DSTANCE	= 1000.0f;
	const float			MIN_CLIPPING_DSTANCE	= 0.01f;
	const D3DXVECTOR3	INIT_CAMERA_POSITION	= { 0.0f, 5.0f, -15.0f };
};

CCameraBase::CCameraBase()
	: m_ViewMatrix			()
	, m_ProjMatrix			()
	, m_LookPosition		( 0.0f, 0.0f, 0.0f )
	, m_ViewVector			( 0.0f, 1.0f, 0.0f )
	, m_ViewAngle			( static_cast<float>(D3DX_PI/4.0) )
	, m_Aspect				( DEFFULT_ASPECT )
	, m_MaxClippingDistance	( MAX_CLIPPING_DSTANCE )
	, m_MinClippingDistance	( MIN_CLIPPING_DSTANCE )
	, m_IsActive			( false )
{
	m_Tranceform.Position = INIT_CAMERA_POSITION;
}

CCameraBase::~CCameraBase()
{
}

//-----------------------------------------------.
// 指定秒で、指定座標に移動するカメラ動作.
//-----------------------------------------------.
void CCameraBase::AnySecondsMove( const SCameraState& newState,const SCameraState& oldState, const float& sec )
{
	const float seconds = sec <= 0.0f ? 0.0f : sec;
	// 現在の座標, 昔の座標, 指定座標,
	D3DXVec3Lerp( &m_Tranceform.Position, &newState.Position, &oldState.Position, seconds );
	D3DXVec3Lerp( &m_LookPosition, &newState.LookPosition, &oldState.LookPosition, seconds );
}

//-----------------------------------------------.
// 指定秒で、指定座標に移動するカメラ動作.
//-----------------------------------------------.
void CCameraBase::AnySecondsMovePosition( const D3DXVECTOR3& newPos, const D3DXVECTOR3& oldPos, const float& sec )
{
	const float seconds = sec <= 0.0f ? 0.0f : sec;
	D3DXVec3Lerp( &m_Tranceform.Position, &newPos, &oldPos, sec );
}

//-----------------------------------------------.
// 指定秒で、指定座標に移動するカメラ動作.
//-----------------------------------------------.
void CCameraBase::AnySecondsMoveLookPosition( const D3DXVECTOR3& newPos, const D3DXVECTOR3& oldPos, const float& sec )
{
	const float seconds = sec <= 0.0f ? 0.0f : sec;
	D3DXVec3Lerp( &m_LookPosition, &newPos, &oldPos, sec );
}


//-----------------------------------------------.
// ビュー・プロジェクションの更新.
//-----------------------------------------------.
void CCameraBase::UpdateViewProj()
{
	// ビュー(カメラ)変換.
	D3DXMatrixLookAtLH( 
		&m_ViewMatrix,				// (out)viewMarix.
		&m_Tranceform.Position,		// カメラ座標.
		&m_LookPosition,			// カメラ注視座標.
		&m_ViewVector );			// カメラベクトル.

	// プロジェクション(射影)変換.
	D3DXMatrixPerspectiveFovLH(
		&m_ProjMatrix,				// (out)ProjMatrix.
		m_ViewAngle,				// 視野角.
		m_Aspect,					// 画面アスペクト比.
		m_MinClippingDistance,		// 最小描画距離.
		m_MaxClippingDistance );	// 最大描画距離.
}