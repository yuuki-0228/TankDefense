/**
* @file LeyInput.h
* @brief This is Use Xinput Controller file.
* @author Fukuta
* @date 2020/12/16
* @details Key情報を取得するためのクラスです.
**/
#ifndef KEY_INPUT_H
#define KEY_INPUT_H

#include <Windows.h>

const int KEY_MAX = 256;	// キーの最大値.

/*******************************
* キー入力を取得するクラス.
**/
class CKeyInput
{

public:
	CKeyInput();
	~CKeyInput();

	// キーの状態の更新.
	static void Update();

	// 押されているとき.
	static bool IsPress			( const unsigned char& key );
	// 押された瞬間.
	static bool IsMomentPress	( const unsigned char& key );
	// 長押ししているとき.
	static bool IsHold			( const unsigned char& key );
	// 離した瞬間.
	static bool IsRelease		( const unsigned char& key );

private:
	// インスタンスの取得.
	static CKeyInput* GetInstance();

	// キーの状態の更新.
	void KeyStateUpdate();
	// キーの状態の取得.
	static unsigned char GetNowState( const unsigned char& key );
	// キーの状態の取得.
	static unsigned char GetOldState( const unsigned char& key );

private:
	BYTE m_NowInputState[KEY_MAX];	// 現在のキーの状態.
	BYTE m_OldInputState[KEY_MAX];	// 前回のキーの状態.

private:
	// コピー・ムーブコンストラクタ, 代入演算子の削除.
	CKeyInput( const CKeyInput & )				= delete;
	CKeyInput( CKeyInput && )					= delete;
	CKeyInput& operator = ( const CKeyInput & )	= delete;
	CKeyInput& operator = ( CKeyInput && )		= delete;
};

#endif	// #ifndef KEY_INPUT_H.