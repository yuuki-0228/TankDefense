#include "Input.h"

CInput::CInput()
	: m_hWnd			( nullptr )
	, m_KeyBindList		()
	, m_AxisBindList	()
	, m_MouseNowPoint	{ 0, 0 }
	, m_MouseOldPoint	{ 0, 0 }
	, m_MouseWheel		( 0 )
{
}

CInput::~CInput()
{
}

//--------------------------------------------------.
// バインドの初期化.
//--------------------------------------------------.
void CInput::InitBind()
{
	InitKeyBind();	// キーバインドの初期化.
	InitAxisBind();	// 軸バインドの初期化.
}

//--------------------------------------------------.
// バインドの初期化.
//--------------------------------------------------.
void CInput::InitBind(
	std::function<void(std::unordered_map<EKeyBind, SKeyBindPair>&)>	keyFunc,
	std::function<void(std::unordered_map<EAxisBind, SAxisBind>&)>		axisFunc )
{
	keyFunc( GetInstance()->m_KeyBindList );	// キーバインドの初期化.
	axisFunc( GetInstance()->m_AxisBindList );	// 軸バインドの初期化.
}

//--------------------------------------------------.
// キーバインドの初期化.
//--------------------------------------------------.
void CInput::InitKeyBind()
{
	GetInstance()->m_KeyBindList =
	{
		// キーバインド列挙体,		設定するキー、ゲームパッドのボタンの設定.
		{ EKeyBind::Decision,		SKeyBindPair( VK_RETURN,	XINPUT_GAMEPAD_B )				},
		{ EKeyBind::Cancel,			SKeyBindPair( VK_BACK,		XINPUT_GAMEPAD_A )				},
		{ EKeyBind::Skip,			SKeyBindPair( VK_RETURN,	XINPUT_GAMEPAD_B )				},
		
		{ EKeyBind::Attack,			SKeyBindPair( 'F',			XINPUT_GAMEPAD_X )				},
		{ EKeyBind::SpecialAbility,	SKeyBindPair( 'Y',			XINPUT_GAMEPAD_Y )				},
		{ EKeyBind::Avoidance,		SKeyBindPair( 'R',			XINPUT_GAMEPAD_A )				},
		{ EKeyBind::STGAttack,		SKeyBindPair( 'Z',			XINPUT_GAMEPAD_RIGHT_SHOULDER )	},

		{ EKeyBind::Up,				SKeyBindPair( VK_UP,		XINPUT_GAMEPAD_DPAD_UP )		},
		{ EKeyBind::Down,			SKeyBindPair( VK_DOWN,		XINPUT_GAMEPAD_DPAD_DOWN )		},
		{ EKeyBind::Right,			SKeyBindPair( VK_RIGHT,		XINPUT_GAMEPAD_DPAD_RIGHT )		},
		{ EKeyBind::Left,			SKeyBindPair( VK_LEFT,		XINPUT_GAMEPAD_DPAD_LEFT )		},

		{ EKeyBind::Start,			SKeyBindPair( VK_ESCAPE,	XINPUT_GAMEPAD_START )			},
		{ EKeyBind::Back,			SKeyBindPair( VK_BACK,		XINPUT_GAMEPAD_BACK )			},

	};
}

//--------------------------------------------------.
// 軸バインドの初期化.
//--------------------------------------------------.
void CInput::InitAxisBind()
{
	GetInstance()->m_AxisBindList = 
	{
		// 軸バインドの列挙体		設定するキー、受け取る軸の関数、受け取る最小値、最大値.
		{ EAxisBind::L_Forward,	SAxisBind( 'S', 'W', [](){ return CXInput::GetLeftThumbY(); }, -1.0f, 1.0f ) },
		{ EAxisBind::L_Right,	SAxisBind( 'A', 'D', [](){ return CXInput::GetLeftThumbX(); }, -1.0f, 1.0f ) },
	};
}

//--------------------------------------------------.
// キーバインドの初期化.
//--------------------------------------------------.
void CInput::InitKeyBind( std::function<void(std::unordered_map<EKeyBind, SKeyBindPair>&)> func )
{
	// 関数を呼び出す.
	func( GetInstance()->m_KeyBindList );
}

//--------------------------------------------------.
// 軸バインドの初期化 : 外部から設定用.
//--------------------------------------------------.
void CInput::InitAxisBind( std::function<void(std::unordered_map<EAxisBind, SAxisBind>&)> func )
{
	// 関数を呼び出す.
	func( GetInstance()->m_AxisBindList );
}

//--------------------------------------------------.
// 入力状態の更新.
//--------------------------------------------------.
void CInput::Update( const float& time )
{
	CXInput::Update( time );
	CKeyInput::Update();
	GetInstance()->UpdateMouse();
}

//--------------------------------------------------.
// 押されているとき.
//--------------------------------------------------.
bool CInput::IsPress( const EKeyBind& key )
{
	return 
		CKeyInput::IsPress( GetInstance()->m_KeyBindList[key].Key ) ||
		CXInput::IsPress( GetInstance()->m_KeyBindList[key].Button );
}

//--------------------------------------------------.
// 押された瞬間.
//--------------------------------------------------.
bool CInput::IsMomentPress( const EKeyBind& key )
{
	return 
		CKeyInput::IsMomentPress( GetInstance()->m_KeyBindList[key].Key ) || 
		CXInput::IsMomentPress( GetInstance()->m_KeyBindList[key].Button );
}

//--------------------------------------------------.
// 長押ししているとき.
//--------------------------------------------------.
bool CInput::IsHold( const EKeyBind& key )
{
	return 
		CKeyInput::IsHold( GetInstance()->m_KeyBindList[key].Key ) ||
		CXInput::IsHold( GetInstance()->m_KeyBindList[key].Button );
}

//--------------------------------------------------.
// 離した瞬間.
//--------------------------------------------------.
bool CInput::IsRelease( const EKeyBind& key )
{
	return 
		CKeyInput::IsRelease( GetInstance()->m_KeyBindList[key].Key ) ||
		CXInput::IsRelease( GetInstance()->m_KeyBindList[key].Button );
}

//--------------------------------------------------.
// 押していない.
//--------------------------------------------------.
bool CInput::NotPress( const EKeyBind& key )
{
	// どっちかが押されていたら falseを返す.
	if( !CKeyInput::IsPress( GetInstance()->m_KeyBindList[key].Key ) == false ||
		!CXInput::IsPress( GetInstance()->m_KeyBindList[key].Button ) == false ){
		return false;
	}
	return true;
}

//--------------------------------------------------.
// 軸値の取得(vector2).
//--------------------------------------------------.
CInput::Vector2 CInput::GetAxisVector( const EAxisBind& key_x, const EAxisBind& key_y )
{
	Vector2 t;
	t.x = GetAxisValue(key_x);
	t.y = GetAxisValue(key_y);

	if( fabsf(t.x) < INPUT_AXIS_DEAD_ZONE && 
		fabsf(t.y) < INPUT_AXIS_DEAD_ZONE ){
		t.x = 0.0f; t.y = 0.0f;
	}

	return t;
}

#ifdef	__D3DX9MATH_H__
//--------------------------------------------------.
// 軸値の取得(D3DXVECTOR2).
//--------------------------------------------------.
D3DXVECTOR2 CInput::GetAxisDxVector( const EAxisBind& key_x, const EAxisBind& key_y )
{
	D3DXVECTOR2 t;
	t.x = GetAxisValue(key_x);
	t.y = GetAxisValue(key_y);

	if( fabsf(t.x) < INPUT_AXIS_DEAD_ZONE && 
		fabsf(t.y) < INPUT_AXIS_DEAD_ZONE ){
		t.x = 0.0f; t.y = 0.0f;
	}

	return t;
}
#endif	// #ifdef __D3DX9MATH_H__

//--------------------------------------------------.
// 軸値の取得.
//--------------------------------------------------.
float CInput::GetAxisValue( const EAxisBind& key )
{
	float value = 0.0f;
	if( CKeyInput::IsPress( GetInstance()->m_AxisBindList[key].PlusKey ) == true ){
		value += GetInstance()->m_AxisBindList[key].MaxValue;
	}
	if( CKeyInput::IsPress( GetInstance()->m_AxisBindList[key].MinusKey ) == true ){
		value += GetInstance()->m_AxisBindList[key].MinValue;
	}
	if( value != 0.0f ) return value;

	// ノーマライズした値を取得.
	value = static_cast<float>(GetInstance()->m_AxisBindList[key].GetValue())/static_cast<float>(SHRT_MAX);
	value = pow( value, INPUT_AXIS_POW_VALUE );
	if( fabsf(value) < 0.01f ) value = 0.0f;

	return value;
}

//--------------------------------------------------.
// マウスがスクリーンの中か.
//--------------------------------------------------.
bool CInput::IsScreenMiddleMouse()
{
	RECT rect;
	if( GetClientRect( GetInstance()->m_hWnd, &rect ) == FALSE ) return false;
	return (( rect.left < GetInstance()->m_MouseNowPoint.x && GetInstance()->m_MouseNowPoint.x < rect.right ) &&
			( rect.top < GetInstance()->m_MouseNowPoint.y && GetInstance()->m_MouseNowPoint.y < rect.bottom ));
}

//--------------------------------------------------.
// インスタンスの取得.
//--------------------------------------------------.
CInput* CInput::GetInstance()
{
	static std::unique_ptr<CInput> pInstance = std::make_unique<CInput>();
	return pInstance.get();
}

//--------------------------------------------------.
// マウスの更新.
//--------------------------------------------------.
void CInput::UpdateMouse()
{
	// 前回を設定.
	m_MouseOldPoint = m_MouseNowPoint;
	// マウスカーソル位置取得.
	GetCursorPos( &m_MouseNowPoint );
	// スクリーン座標→クライアント座標に変換.
	ScreenToClient( m_hWnd, &m_MouseNowPoint );
}