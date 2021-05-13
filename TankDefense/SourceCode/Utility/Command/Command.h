/**
* @file Command.h.
* @brief 入力コマンドを管理するクラス.
* @author 福田玲也.
*/
#ifndef COMMAND_H
#define COMMAND_H

#include <queue>
#include <vector>

/****************************************
*	入力コマンド情報を管理するクラス.
*		Cppファイルの方で任意のコマンドを設定できる.
**/
class CCommand
{
public:
	// キー(button)のリスト.
	enum enKey : unsigned char
	{
		EKey_Up,	// 上.
		EKey_Down,	// 下.
		EKey_Right,	// 右.
		EKey_Left,	// 左.
		EKey_A,		// A.
		EKey_B,		// B.
		EKey_X,		// X.
		EKey_Y,		// Y.
		EKey_RB,	// RB.
		EKey_LB,	// LB.
	} typedef EKey;
	
	using command_list = std::vector<CCommand::EKey>;

public:
	CCommand();
	CCommand( const command_list& );
	~CCommand();

	void Update();

	// 成功したか.
	inline bool IsSuccess() { return m_isSuccessCommand; }

private:
	// キー(buttonの入力).
	void InputKey();
	// キーの追加.
	void PushKey( const EKey& key );
	// キューの確認.
	void Check();

private:
	const command_list	COMMAND_LIST;		// コマンドのリスト.
	std::queue<EKey>	m_KeyQueue;			// 入力されたキーのキュー.
	int					m_TimeCount;		// 入力されてからのタイムカウント.
	bool				m_isSuccessCommand;	// コマンドが成功したか.
};

#endif	// #ifndef COMMAND_H.