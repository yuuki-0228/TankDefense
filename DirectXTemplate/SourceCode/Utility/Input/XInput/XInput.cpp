#include "XInput.h"
#include <algorithm>

CXInput::CXInput()
	: m_XinputState	()
{
}

CXInput::~CXInput()
{
	for( int i = 0; i < MAX_CONTROLLERS; i++ ){
		m_XinputState[i].Vibration.wLeftMotorSpeed	= 0;
		m_XinputState[i].Vibration.wRightMotorSpeed	= 0;
		// バイブレーションの値の設定.
		XInputSetState( i, &m_XinputState[i].Vibration );
	}
}

//------------------------------------------------------.
// 更新.
//------------------------------------------------------.
void CXInput::Update( const float& time )
{
	GetInstance()->StateUpdate();
	GetInstance()->VibrationUpdate( time );
}

//------------------------------------------------------.
// 押されているとき.
//------------------------------------------------------.
bool CXInput::IsPress( const WORD& buttonMask, const UCHAR& connectNum )
{
	if( GetInstance()->ControllerValid( connectNum ) == false ) return false;	// コントローラーが無効なら終了.

	// 指定したボタンの状態のチェック.
	if( GetInstance()->m_XinputState[connectNum].NowState.Gamepad.wButtons & buttonMask ) return true;
	return false;
}

//------------------------------------------------------.
// 押した瞬間.
//------------------------------------------------------.
bool CXInput::IsMomentPress( const WORD& buttonMask, const UCHAR& connectNum )
{
	if( GetInstance()->ControllerValid( connectNum ) == false ) return false;	// コントローラーが無効なら終了.

	// 現在入力で前回入力してなければ.
	if( ( GetInstance()->m_XinputState[connectNum].NowState.Gamepad.wButtons & buttonMask ) != 0 && 
		( GetInstance()->m_XinputState[connectNum].OldState.Gamepad.wButtons & buttonMask ) == 0 ){
			return true;
	}
	return false;
}

//------------------------------------------------------.
// 長押ししているとき.
//------------------------------------------------------.
bool CXInput::IsHold( const WORD& buttonMask, const UCHAR& connectNum )
{
	if( GetInstance()->ControllerValid( connectNum ) == false ) return false;	// コントローラーが無効なら終了.

	// 現在も入力で前回も入力なら.
	if( ( GetInstance()->m_XinputState[connectNum].NowState.Gamepad.wButtons & buttonMask ) != 0 && 
		( GetInstance()->m_XinputState[connectNum].OldState.Gamepad.wButtons & buttonMask ) != 0 ){
		return true;
	}
	return false;
}

//------------------------------------------------------.
// 離した瞬間.
//------------------------------------------------------.
bool CXInput::IsRelease( const WORD& buttonMask, const UCHAR& connectNum )
{
	if( GetInstance()->ControllerValid( connectNum ) == false ) return false;	// コントローラーが無効なら終了.

	// 現在入力してなく前回入力していたら.
	if( ( GetInstance()->m_XinputState[connectNum].NowState.Gamepad.wButtons & buttonMask ) == 0 && 
		( GetInstance()->m_XinputState[connectNum].OldState.Gamepad.wButtons & buttonMask ) != 0 ){
		return true;
	}
	return false;
}

//------------------------------------------------------.
// 左トリガー取得.
//------------------------------------------------------.
BYTE CXInput::GetLeftTrigger( const UCHAR& connectNum )
{
	if( GetInstance()->ControllerValid( connectNum ) == false ) return 0;	// コントローラーが無効なら終了.
	return GetInstance()->m_XinputState[connectNum].NowState.Gamepad.bLeftTrigger;
}

//------------------------------------------------------.
// 右トリガー取得.
//------------------------------------------------------.
BYTE CXInput::GetRightTrigger( const UCHAR& connectNum )
{
	if( GetInstance()->ControllerValid( connectNum ) == false ) return 0;	// コントローラーが無効なら終了.
	return GetInstance()->m_XinputState[connectNum].NowState.Gamepad.bRightTrigger;
}

//------------------------------------------------------.
// 左スティックX軸取得.
//------------------------------------------------------.
SHORT CXInput::GetLeftThumbX( const UCHAR& connectNum )
{
	if( GetInstance()->ControllerValid( connectNum ) == false ) return 0;	// コントローラーが無効なら終了.
	return GetInstance()->m_XinputState[connectNum].NowState.Gamepad.sThumbLX;
}

//------------------------------------------------------.
// 左スティックY軸取得.
//------------------------------------------------------.
SHORT CXInput::GetLeftThumbY( const UCHAR& connectNum )
{
	if( GetInstance()->ControllerValid( connectNum ) == false ) return 0;	// コントローラーが無効なら終了.
	return GetInstance()->m_XinputState[connectNum].NowState.Gamepad.sThumbLY;
}

//------------------------------------------------------.
// 右スティックX軸取得.
//------------------------------------------------------.
SHORT CXInput::GetRightThumbX( const UCHAR& connectNum )
{
	if( GetInstance()->ControllerValid( connectNum ) == false ) return 0;	// コントローラーが無効なら終了.
	return GetInstance()->m_XinputState[connectNum].NowState.Gamepad.sThumbRX;
}

//------------------------------------------------------.
// 右スティックY軸取得.
//------------------------------------------------------.
SHORT CXInput::GetRightThumbY( const UCHAR& connectNum ) 
{
	if( GetInstance()->ControllerValid( connectNum ) == false ) return 0;	// コントローラーが無効なら終了.
	return GetInstance()->m_XinputState[connectNum].NowState.Gamepad.sThumbRY;
}

//------------------------------------------------------.
// バイブレーションの設定.
//------------------------------------------------------.
void CXInput::SetVibration( const WORD& rightMotorSpd, const WORD& leftMotorSpd, const float& time, const UCHAR& connectNum )
{
	if( GetInstance()->ControllerValid( connectNum ) == false ) return;	// コントローラーが無効なら終了.

	GetInstance()->m_XinputState[connectNum].Vibration.wRightMotorSpeed	= std::clamp<WORD>( rightMotorSpd, INPUT_VIBRATION_MIN, INPUT_VIBRATION_MAX );;
	GetInstance()->m_XinputState[connectNum].Vibration.wLeftMotorSpeed	= std::clamp<WORD>( leftMotorSpd, INPUT_VIBRATION_MIN, INPUT_VIBRATION_MAX );;
	GetInstance()->m_XinputState[connectNum].VibrationTimeRight	= time;
	GetInstance()->m_XinputState[connectNum].VibrationTimeLeft	= time;
}

//------------------------------------------------------.
// 右のバイブレーションの設定.
//------------------------------------------------------.
void CXInput::SetRightVibration ( const WORD& motorSpd, const float& time, const UCHAR& connectNum )
{
	if( GetInstance()->ControllerValid( connectNum ) == false ) return;	// コントローラーが無効なら終了.

	GetInstance()->m_XinputState[connectNum].Vibration.wRightMotorSpeed	= std::clamp<WORD>( motorSpd, INPUT_VIBRATION_MIN, INPUT_VIBRATION_MAX );
	GetInstance()->m_XinputState[connectNum].VibrationTimeRight	= time;
}

//------------------------------------------------------.
// 左のバイブレーションの設定.
//------------------------------------------------------.
void CXInput::SetLeftVibration ( const WORD& motorSpd, const float& time, const UCHAR& connectNum )
{
	if( GetInstance()->ControllerValid( connectNum ) == false ) return;	// コントローラーが無効なら終了.

	GetInstance()->m_XinputState[connectNum].Vibration.wLeftMotorSpeed	= std::clamp<WORD>( motorSpd, INPUT_VIBRATION_MIN, INPUT_VIBRATION_MAX );
	GetInstance()->m_XinputState[connectNum].VibrationTimeLeft	= time;
}

//------------------------------------------------------.
// 状態の更新.
//------------------------------------------------------.
void CXInput::StateUpdate()
{
	DWORD result;	// 接続状態.
	for( int i = 0; i < MAX_CONTROLLERS; i++ ){
		// 現在の状態を保存し新しい状態を取得する.
		m_XinputState[i].OldState = m_XinputState[i].NowState;
		result = XInputGetState( i, &m_XinputState[i].NowState );
		m_XinputState[i].Connected = result == ERROR_SUCCESS ? true : false;
	}
}

//------------------------------------------------------.
// バイブレーションの更新.
//------------------------------------------------------.
void CXInput::VibrationUpdate( const float& time )
{
	for( int i = 0; i < MAX_CONTROLLERS; i++ ){
		if( m_XinputState[i].Connected == false ) continue;

		// 経過時間を減らす.
		m_XinputState[i].VibrationTimeRight	-= time;
		m_XinputState[i].VibrationTimeLeft	-= time;

		// 経過時間が 0.0f よりちいさくなれば.
		//	バイブレーションを止める.
		if( m_XinputState[i].VibrationTimeRight < 0.0f ){
			m_XinputState[i].VibrationTimeRight = 0.0f;
			m_XinputState[i].Vibration.wRightMotorSpeed = 0;
		}
		if( m_XinputState[i].VibrationTimeLeft < 0.0f ){
			m_XinputState[i].VibrationTimeLeft = 0.0f;
			m_XinputState[i].Vibration.wLeftMotorSpeed = 0;
		}
		// バイブレーションの値の設定.
		XInputSetState( i, &m_XinputState[i].Vibration );
	}
}

//------------------------------------------------------.
// 指定したコントローラーが有効か.
//------------------------------------------------------.
bool CXInput::ControllerValid( const UCHAR& connectNum ) const
{
	if( connectNum > MAX_CONTROLLER_ARRAY_LEN )			return false;	// 接続数が一定値より大きければ false.
	if( m_XinputState[connectNum].Connected == false )	return false;	// コントーラーが接続してなけらば false.

	return true;
}
