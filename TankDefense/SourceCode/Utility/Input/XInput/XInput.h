#ifndef XINPUT_H
#define XINPUT_H

#include <Windows.h>
#include <XInput.h>
#include <vector>
#include <unordered_map>
#include <memory>

#pragma comment( lib, "xinput.lib" )

struct stXInputState
{
	XINPUT_STATE		NowState;			// 現在のXinputの状態.
	XINPUT_STATE		OldState;			// 前回のXinputの状態.
	XINPUT_VIBRATION	Vibration;			// Xinputのバイブレーションの状態.
	float				VibrationTimeRight;	// 左のバイブレーションの計測時間.
	float				VibrationTimeLeft;	// 左のバイブレーションの計測時間.	
	bool				Connected;			// 接続しているか.

	stXInputState()
		: NowState				()
		, OldState				()
		, Vibration				()
		, VibrationTimeRight	( 0.0f )
		, VibrationTimeLeft		( 0.0f )
		, Connected				( false )
	{}

} typedef SXInputState;


const UCHAR MAX_CONTROLLERS				= 4;					// コントローラーの最大接続数.
const UCHAR MAX_CONTROLLER_ARRAY_LEN	= MAX_CONTROLLERS-1;	// コントローラーの配列の最大数.
const SHORT	IDLE_THUMB_MAX				=  10000;				// スティックの遊び.
const SHORT	IDLE_THUMB_MIN				= -10000;				// スティックの遊び.
const BYTE	IDLE_TIGGER_MAX				=  100;					// トリガーボタンの遊び.
const WORD	INPUT_VIBRATION_MIN			=  0;					// バイブレーションの最小値.
const WORD	INPUT_VIBRATION_MAX			=  65535;				// バイブレーションの最大値.

class CXInput
{
public:
	CXInput();
	~CXInput();

	// 更新.
	static void Update( const float& time );

	// 押されているとき.
	static bool IsPress			( const WORD& buttonMask, const UCHAR& connectNum = 0 );
	// 押した瞬間.
	static bool IsMomentPress	( const WORD& buttonMask, const UCHAR& connectNum = 0 );
	// 長押ししているとき.
	static bool IsHold			( const WORD& buttonMask, const UCHAR& connectNum = 0 );
	// 離した瞬間.
	static bool IsRelease		( const WORD& buttonMask, const UCHAR& connectNum = 0 );

	// 左トリガー取得.
	static BYTE GetLeftTrigger	( const UCHAR& connectNum = 0 );
	// 右トリガー取得.
	static BYTE GetRightTrigger	( const UCHAR& connectNum = 0 );

	// 左スティックX軸取得.
	static SHORT GetLeftThumbX	( const UCHAR& connectNum = 0 );
	// 左スティックY軸取得.
	static SHORT GetLeftThumbY	( const UCHAR& connectNum = 0 );

	// 右スティックX軸取得.
	static SHORT GetRightThumbX	( const UCHAR& connectNum = 0 );
	// 右スティックY軸取得.
	static SHORT GetRightThumbY	( const UCHAR& connectNum = 0 );

	// バイブレーションの設定.
	static void SetVibration		( const WORD& rightMotorSpd, const WORD& leftMotorSpd, const float& time, const UCHAR& connectNum = 0 );
	// 右のバイブレーションの設定.
	static void SetRightVibration	( const WORD& motorSpd, const float& time, const UCHAR& connectNum = 0 );
	// 左のバイブレーションの設定.
	static void SetLeftVibration	( const WORD& motorSpd, const float& time, const UCHAR& connectNum = 0 );

private:
	// 状態の更新.
	void StateUpdate();
	// バイブレーションの更新.
	void VibrationUpdate( const float& time );

	// 指定したコントローラーが有効か.
	bool ControllerValid( const UCHAR& connectNum ) const;

	static CXInput* GetInstance()
	{
		static std::unique_ptr<CXInput> pInstance = std::make_unique<CXInput>();
		return pInstance.get();
	}

private:
	SXInputState	m_XinputState[MAX_CONTROLLERS];	// コンローラーの接続状態.
	
private:
	// コピー・ムーブコンストラクタ, 代入演算子の削除.
	CXInput( const CXInput & )				= delete;
	CXInput( CXInput && )					= delete;
	CXInput& operator = ( const CXInput & )	= delete;
	CXInput& operator = ( CXInput && )		= delete;
};


#endif	// #ifndef XINPUT_H.