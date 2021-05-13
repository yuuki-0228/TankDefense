#include "FreeCamera.h"
#include "..\..\..\Utility\Input\Input.h"

namespace
{
	const float CAMERA_MOVE_SPEED				= 8.0f;		// カメラの移動速度.
	const float CAMERA_POS_CAMERA_LOOK_DISTANCE = 15.0f;	// カメラと視点の距離.
	const float MOUSE_MOVE_SPEED				= 2.0f;		// マウスの移動速度.
	const float CONTROLLER_MOVE_SPEED			= 2.0f;		// コントローラーの移動速度.
	const float RADIAN_THRESHOLD_X_MAX			= 360.0f;	// ラジアンXの最大しきい値.
	const float RADIAN_THRESHOLD_X_MIN			= 0.0f;		// ラジアンXの最小しきい値.
	const float RADIAN_THRESHOLD_Y_MAX			= 90.0f;	// ラジアンYの最大しきい値.
	const float RADIAN_THRESHOLD_Y_MIN			= -90.0f;	// ラジアンYの最小しきい値.
	const D3DXVECTOR3	INIT_POSITION			= { 0.0f, 10.0f, -30.0f };	// 初期座標.
}

CFreeCamera::CFreeCamera()
	: m_Radian		( 0.0f, 0.0f )
	, m_DeltaTime	( 0.0f )
{
}

CFreeCamera::~CFreeCamera()
{
}

//------------------------------.
// 更新関数.
//------------------------------.
void CFreeCamera::Update( const float& deltaTime )
{
	if( CKeyInput::IsHold(VK_LCONTROL) == false ) return;

	m_DeltaTime = deltaTime;

	D3DXMATRIX mRot;
	D3DXMatrixRotationYawPitchRoll( &mRot, m_Radian.x, 0.0f, 0.0f );
	// 軸ベクトルを用意.
	D3DXVECTOR3 vecAxisX( CAMERA_MOVE_SPEED, 0.0f, 0.0f );	// X軸.
	D3DXVECTOR3 vecAxisZ( 0.0f, 0.0f, CAMERA_MOVE_SPEED );	// Z軸.
	// X軸ベクトルそのものを回転状態により変換する.
	D3DXVec3TransformCoord( &vecAxisX, &vecAxisX, &mRot );
	// Z軸ベクトルそのものを回転状態により変換する.
	D3DXVec3TransformCoord( &vecAxisZ, &vecAxisZ, &mRot );

	// 前進.
	if( CKeyInput::IsHold('W') == true || CXInput::GetLeftThumbY() > IDLE_THUMB_MAX )
		m_Tranceform.Position += vecAxisZ * m_DeltaTime;
	// 後退.
	if( CKeyInput::IsHold('S') == true || CXInput::GetLeftThumbY() < IDLE_THUMB_MIN )
		m_Tranceform.Position -= vecAxisZ * m_DeltaTime;
	// 右に移動.
	if( CKeyInput::IsHold('D') == true || CXInput::GetLeftThumbX() > IDLE_THUMB_MAX )
		m_Tranceform.Position += vecAxisX * m_DeltaTime;
	// 左に移動.
	if( CKeyInput::IsHold('A') == true || CXInput::GetLeftThumbX() < IDLE_THUMB_MIN )
		m_Tranceform.Position -= vecAxisX * m_DeltaTime;
	// 上昇.
	if( CKeyInput::IsHold('E') == true || CXInput::GetRightTrigger() > IDLE_TIGGER_MAX ) 
		m_Tranceform.Position.y += CAMERA_MOVE_SPEED * m_DeltaTime;
	// 下降.
	if( CKeyInput::IsHold('Q') == true || CXInput::GetLeftTrigger() > IDLE_TIGGER_MAX )
		m_Tranceform.Position.y -= CAMERA_MOVE_SPEED * m_DeltaTime;

	MouseUpdate();

	// 注視位置を算出.
	m_LookPosition.x = m_Tranceform.Position.x + ( sinf(m_Radian.x) * CAMERA_POS_CAMERA_LOOK_DISTANCE );
	m_LookPosition.y = m_Tranceform.Position.y + ( sinf(m_Radian.y) * CAMERA_POS_CAMERA_LOOK_DISTANCE );
	m_LookPosition.z = m_Tranceform.Position.z + ( cosf(m_Radian.x) * CAMERA_POS_CAMERA_LOOK_DISTANCE );
}

//------------------------------.
// マウスの更新.
//------------------------------.
void CFreeCamera::MouseUpdate()
{
	float xSub = 0.0f;
	float ySub = 0.0f;
	float moveSpeed = MOUSE_MOVE_SPEED * m_DeltaTime;
	if( CXInput::GetRightThumbY() > IDLE_THUMB_MAX )	ySub = -static_cast<float>(CXInput::GetRightThumbY());
	if( CXInput::GetRightThumbY() < IDLE_THUMB_MIN )	ySub = -static_cast<float>(CXInput::GetRightThumbY());
	if( CXInput::GetRightThumbX() > IDLE_THUMB_MAX )	xSub =  static_cast<float>(CXInput::GetRightThumbX());
	if( CXInput::GetRightThumbX() < IDLE_THUMB_MIN )	xSub =  static_cast<float>(CXInput::GetRightThumbX());
	if( CKeyInput::IsPress(VK_LBUTTON) == false ){
		moveSpeed = CONTROLLER_MOVE_SPEED * m_DeltaTime;	// 移動速度を半分にする.
	} else {
		// マウスが画面外なら終了.
		if( CInput::IsScreenMiddleMouse() == false ) return;
		// マウスの現在の座標と過去の座標を引いた値を算出.
		if( ySub == 0.0f ) ySub = CInput::GetMousePosisionY() - CInput::GetMouseOldPosisionY();
		if( xSub == 0.0f ) xSub = CInput::GetMousePosisionX() - CInput::GetMouseOldPosisionX();
	}


	if( xSub < 0.0f ) m_Radian.x -= moveSpeed;
	if( xSub > 0.0f ) m_Radian.x += moveSpeed;
	if( ySub > 0.0f ) m_Radian.y -= moveSpeed;
	if( ySub < 0.0f ) m_Radian.y += moveSpeed;
	// 規定値を超えないよう調整.
	if( m_Radian.x > static_cast<float>(D3DXToRadian(RADIAN_THRESHOLD_X_MAX)) )
		m_Radian.x = static_cast<float>(D3DXToRadian(RADIAN_THRESHOLD_X_MIN));
	if( m_Radian.x < static_cast<float>(D3DXToRadian(RADIAN_THRESHOLD_X_MIN)) ) 
		m_Radian.x = static_cast<float>(D3DXToRadian(RADIAN_THRESHOLD_X_MAX));
	if( m_Radian.y > static_cast<float>(D3DXToRadian(RADIAN_THRESHOLD_Y_MAX)) )
		m_Radian.y = static_cast<float>(D3DXToRadian(RADIAN_THRESHOLD_Y_MAX));
	if( m_Radian.y < static_cast<float>(D3DXToRadian(RADIAN_THRESHOLD_Y_MIN)) )
		m_Radian.y = static_cast<float>(D3DXToRadian(RADIAN_THRESHOLD_Y_MIN));
}