#ifndef INPUT_H
#define INPUT_H

#include <unordered_map>
#include <functional>
#include <memory>
#include <Windows.h>

#include "InputEnums.h"
#include "XInput/XInput.h"
#include "KeyInput/KeyInput.h"
#include "..\..\Global.h"

// 入力軸の無効範囲.
inline static const float	INPUT_AXIS_DEAD_ZONE = 0.4f;
// 入力軸のn乗値.
// コントローラーから受け取った値をn乗すると.
// 値が滑らかになる.
// https://hexadrive.jp/lab/tips/976/.
inline static const int		INPUT_AXIS_POW_VALUE = 3;

class CInput
{
public:
	// キーバインドのペア構造体.
	struct stKeyBindPair
	{
		const unsigned char	Key;	// キー番号.
		const WORD			Button;	// Xinputのボタン番号.

		stKeyBindPair()
			: stKeyBindPair	( 0, 0 )
		{}
		stKeyBindPair(
			const unsigned char&	k,
			const WORD&				b )
			: Key		( k )
			, Button	( b )
		{}

	};

	// 軸バインド.
	struct stAxisBind
	{
		const unsigned char	MinusKey;	// マイナス方向で受け取るキー番号.
		const unsigned char	PlusKey;	// プラス方向で受け取るキー番号.

		const std::function<SHORT()> GetValue;// コントローラの軸値の取得関数.

		const float MinValue;	// キーで受け取る際の最小値.
		const float MaxValue;	// キーで受け取る際の最大値.

		stAxisBind() : stAxisBind	( 0, 0, [](){ return 0; }, 0.0f, 0.0f ){}
		stAxisBind(
			const unsigned char& mk,
			const unsigned char& pk,
			const std::function<SHORT()>& func,
			const float& minValue,
			const float& maxValue )
			: MinusKey	( mk )
			, PlusKey	( pk )
			, GetValue	( func )
			, MinValue	( minValue )
			, MaxValue	( maxValue )
		{}
	};

	struct Vector2
	{
		float x;
		float y;
		float length;

		Vector2() : x (0), y (0), length (0){}
	};

	using SKeyBindPair	= stKeyBindPair;
	using SAxisBind		= stAxisBind;
public:
	CInput();
	~CInput();

	// ウィンドウハンドルの設定.
	static void SetHWND( HWND hWnd ){ GetInstance()->m_hWnd = hWnd; }

	// バインドの初期化.
	static void InitBind();
	// バインドの初期化 : 外部から設定用.
	static void InitBind(
		std::function<void(std::unordered_map<EKeyBind, SKeyBindPair>&)>	keyFunc,
		std::function<void(std::unordered_map<EAxisBind, SAxisBind>&)>		axisFunc );

	// キーバインドの初期化.
	static void InitKeyBind();
	// 軸バインドの初期化.
	static void InitAxisBind();
	// キーバインドの初期化 : 外部から設定用.
	// 以下使用例.
	/*
		CInput::InitKeyBind( []( std::unordered_map<EKeyBind, CInput::SKeyBindPair>& list )
		{
			list =
			{
				キーバインド列挙体,		設定するキー、ゲームパッドのボタンの設定.
				{ EKeyBind::Decision,	CInput::SKeyBindPair( VK_RETURN,	XINPUT_GAMEPAD_B )	},
				{ EKeyBind::Cancel,		CInput::SKeyBindPair( VK_BACK,		XINPUT_GAMEPAD_A )	},
			};
		};
	*/
	static void InitKeyBind( std::function<void(std::unordered_map<EKeyBind, SKeyBindPair>&)> func );
	// 軸バインドの初期化 : 外部から設定用.
	// 以下使用例.
	/*
		CInput::InitAxisBind( []( std::unordered_map<EAxisBind, SAxisBind>& list )
		{
			list =
			{
				軸バインドの列挙体		設定するキー、受け取る軸の関数、受け取る最小値、最大値.
				{ EAxisBind::L_Forward,	SAxisBind( 'S', 'W', [](){ return CXInput::LThumbY_Axis(); }, -1.0f, 1.0f ) },
				{ EAxisBind::L_Right,	SAxisBind( 'A', 'D', [](){ return CXInput::LThumbX_Axis(); }, -1.0f, 1.0f ) },
			};
		};
	*/
	static void InitAxisBind( std::function<void(std::unordered_map<EAxisBind, SAxisBind>&)> func );

	// 入力状態の更新.
	static void Update( const float& time );

	// 押されているとき.
	static bool IsPress( const EKeyBind& key );
	// 押された瞬間.
	static bool IsMomentPress( const EKeyBind& key );
	// 長押ししているとき.
	static bool IsHold( const EKeyBind& key );
	// 離した瞬間.
	static bool IsRelease( const EKeyBind& key );
	// 押していない.
	static bool NotPress( const EKeyBind& key );

	// 軸値の取得(vector2).
	static Vector2 GetAxisVector( const EAxisBind& key_x, const EAxisBind& key_y );
#ifdef	__D3DX9MATH_H__	//「D3DX～」系が呼ばれいれば以下の関数が使用できる.
	// 軸値の取得(D3DXVECTOR2).
	static D3DXVECTOR2 GetAxisDxVector( const EAxisBind& key_x, const EAxisBind& key_y );
#endif	// #ifdef __D3DX9MATH_H__

	// 軸値の取得.
	static float GetAxisValue( const EAxisBind& key );

	//-----------------------------------------------.
	// ↓ マウス系 ↓
	//-----------------------------------------------.

	// 座標の取得.
	static POINT GetMousePosition()		{ return GetInstance()->m_MouseNowPoint; }
	// 前回の座標の取得.
	static POINT GetMouseOldPosition()	{ return GetInstance()->m_MouseOldPoint; }
	// 座標Xの取得.
	static float GetMousePosisionX()	{ return static_cast<float>(GetInstance()->m_MouseNowPoint.x); }
	// 座標Yの取得.
	static float GetMousePosisionY()	{ return static_cast<float>(GetInstance()->m_MouseNowPoint.y); }
	// 前回の座標Xの取得.
	static float GetMouseOldPosisionX()	{ return static_cast<float>(GetInstance()->m_MouseOldPoint.x); }
	// 前回の座標Yの取得.
	static float GetMouseOldPosisionY()	{ return static_cast<float>(GetInstance()->m_MouseOldPoint.y); }

	// マウスがスクリーンの中か.
	static bool IsScreenMiddleMouse();

	// マウスホイールの値の設定.
	static void SetMouseWheelDelta( const int& delta ){ GetInstance()->m_MouseWheel = delta; }
	// マウスホイールの値の取得.
	//	+ 上方向への移動.
	//	- 下方向への移動.
	static int GetMouseWheelDelta(){ return GetInstance()->m_MouseWheel; }

private:
	// インスタンスの取得.
	static CInput* GetInstance();

	// マウスの更新.
	void UpdateMouse();

private:
	HWND										m_hWnd;				// ウィンドウハンドル.
	std::unordered_map<EKeyBind, SKeyBindPair>	m_KeyBindList;		// KeyBindPairのリスト.
	std::unordered_map<EAxisBind, SAxisBind>	m_AxisBindList;		// AxisBindのリスト.
	POINT										m_MouseNowPoint;	// 現在のマウス座標.
	POINT										m_MouseOldPoint;	// 前回のマウス座標.
	int											m_MouseWheel;		// マウスホイール値.
private:
	// コピー・ムーブコンストラクタ, 代入演算子の削除.
	CInput( const CInput & )				= delete;
	CInput( CInput && )						= delete;
	CInput& operator = ( const CInput & )	= delete;
	CInput& operator = ( CInput && )		= delete;
};

#endif	// #ifndef INPUT_H.