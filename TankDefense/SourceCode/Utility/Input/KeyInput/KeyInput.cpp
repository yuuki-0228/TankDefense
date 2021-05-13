#include "KeyInput.h"

#include <Windows.h>
#include <memory>

CKeyInput::CKeyInput()
	: m_NowInputState() 
{
	for( auto& k : m_NowInputState ) k = 0;
	for( auto& k : m_OldInputState ) k = 0;
}

CKeyInput::~CKeyInput()
{}

// キーの状態の更新.
void CKeyInput::Update()
{
	GetInstance()->KeyStateUpdate();
}

// 押されているとき.
bool CKeyInput::IsPress( const unsigned char& key )
{
	if( GetNowState( key ) & 0x80 ) return true;
	return false;
}

// 押された瞬間.
bool CKeyInput::IsMomentPress( const unsigned char& key )
{
	// 現在入力で前回入力してなければ.
	if( ( GetNowState( key ) & 0x80 ) != 0 &&
		( GetOldState( key ) & 0x80 ) == 0){
		return true;
	}
	return false;
}

// 長押ししているとき.
bool CKeyInput::IsHold( const unsigned char& key )
{
	// 現在も入力で前回も入力なら.
	if( ( GetNowState( key ) & 0x80 ) != 0 &&
		( GetOldState( key ) & 0x80 ) != 0){
		return true;
	}
	return false;
}

// 離した瞬間.
bool CKeyInput::IsRelease( const unsigned char& key )
{
	// 現在も入力で前回も入力なら.
	if( ( GetNowState( key ) & 0x80 ) == 0 &&
		( GetOldState( key ) & 0x80 ) != 0){
		return true;
	}
	return false;
}

// インスタンスの取得.
CKeyInput* CKeyInput::GetInstance()
{
	static std::unique_ptr<CKeyInput> pInstance = std::make_unique<CKeyInput>();
	return pInstance.get();
}

// キーの状態の更新.
void CKeyInput::KeyStateUpdate()
{
	memcpy_s( m_OldInputState, sizeof(m_OldInputState), m_NowInputState, sizeof(m_NowInputState) );
	GetKeyboardState( m_NowInputState );
};

// キーの状態の取得.
unsigned char CKeyInput::GetNowState( const unsigned char& key )
{
	return GetInstance()->m_NowInputState[key];
}

// キーの状態の取得.
unsigned char CKeyInput::GetOldState( const unsigned char& key )
{
	return GetInstance()->m_OldInputState[key];
}
