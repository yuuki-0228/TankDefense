#include "Command.h"
#include "..\Input\Input.h"

namespace
{
// コマンドのリスト.
const CCommand::command_list DEFAULT_COMMAND_LIST = 
{
	CCommand::EKey_Up,
	CCommand::EKey_Up,
	CCommand::EKey_Down,
	CCommand::EKey_Down,
	CCommand::EKey_Left,
	CCommand::EKey_Right,
	CCommand::EKey_Left,
	CCommand::EKey_Right,
	CCommand::EKey_RB,
	CCommand::EKey_LB,
};

const int COMMAND_RESET_FRAME = 30;	// コマンドのリセットフレーム.
}

CCommand::CCommand()
	: CCommand	( DEFAULT_COMMAND_LIST )
{
}

CCommand::CCommand( const command_list& command )
	: COMMAND_LIST			( command )
	, m_KeyQueue			()
	, m_TimeCount			( 0 )
	, m_isSuccessCommand	( false )
{
}

CCommand::~CCommand()
{
}

void CCommand::Update()
{
	m_isSuccessCommand = false;
	InputKey();	// キーの入力.
	Check();	// 確認.
	m_TimeCount++;
	if( m_TimeCount >= COMMAND_RESET_FRAME ){
		m_TimeCount = 0;
		for( int i = 0; i < static_cast<int>(m_KeyQueue.size()); i++ ) m_KeyQueue.pop();
	}
}

// キー(buttonの入力).
void CCommand::InputKey()
{
	if( CKeyInput::IsMomentPress(VK_UP) == true || CXInput::IsMomentPress(XINPUT_GAMEPAD_DPAD_UP) == true )
		PushKey( EKey_Up );
	if( CKeyInput::IsMomentPress(VK_DOWN) == true || CXInput::IsMomentPress(XINPUT_GAMEPAD_DPAD_DOWN) == true )
		PushKey( EKey_Down );
	if( CKeyInput::IsMomentPress(VK_RIGHT) == true || CXInput::IsMomentPress(XINPUT_GAMEPAD_DPAD_RIGHT) == true )
		PushKey( EKey_Right );
	if( CKeyInput::IsMomentPress(VK_LEFT) == true || CXInput::IsMomentPress(XINPUT_GAMEPAD_DPAD_LEFT) == true )
		PushKey( EKey_Left );
	if( CKeyInput::IsMomentPress('A') == true || CXInput::IsMomentPress(XINPUT_GAMEPAD_A) == true )
		PushKey( EKey_A );
	if( CKeyInput::IsMomentPress('B') == true || CXInput::IsMomentPress(XINPUT_GAMEPAD_B) == true )
		PushKey( EKey_B );
	if( CKeyInput::IsMomentPress('X') == true || CXInput::IsMomentPress(XINPUT_GAMEPAD_X) == true )
		PushKey( EKey_X );
	if( CKeyInput::IsMomentPress('Y') == true || CXInput::IsMomentPress(XINPUT_GAMEPAD_Y) == true )
		PushKey( EKey_Y );
	if( CKeyInput::IsMomentPress('R') == true || CXInput::IsMomentPress(XINPUT_GAMEPAD_RIGHT_SHOULDER) == true )
		PushKey( EKey_RB );
	if( CKeyInput::IsMomentPress('L') == true || CXInput::IsMomentPress(XINPUT_GAMEPAD_LEFT_SHOULDER) == true )
		PushKey( EKey_LB );
}

// キーの追加.
void CCommand::PushKey( const EKey& key )
{
	m_KeyQueue.push( key );
	m_TimeCount = 0;
}

// キューの確認.
void CCommand::Check()
{
	// キューのサイズをリストのサイズを比較.
	if( COMMAND_LIST.size() != m_KeyQueue.size() ) return;

	bool faild = false;
	for( int i = 0; i < static_cast<int>(COMMAND_LIST.size()); i++ ){
		if( COMMAND_LIST[i] == m_KeyQueue.front() ){
			m_isSuccessCommand = true;
		} else {
			m_isSuccessCommand = false;
			faild = true;
		}
		m_KeyQueue.pop();
	}
	if( faild == false ) return;
	m_isSuccessCommand = false;
}