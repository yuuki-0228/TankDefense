/**
* @file Log.h.
* @brief ログファイル出力クラス.
* @author 福田玲也.
*/
#ifndef LOG_H
#define LOG_H

#include <Windows.h>
#include <memory>
#include <string>
#include <time.h>

/****************************************************************
*	ログ出力クラス.
*	・OpenLogTextFile()をアプリを起動したタイミングで呼び出す.
*	・CloseLogTextFile()をアプリを終了するタイミングで呼び出す.
*	・Print( message )で書き残したいメッセージをいれる.
**/
class CLog
{
	typedef tm TIME_DATA;
	const char* LOG_TEXT_FILE_PATH = "Data\\Log.txt";	// ログテキストのファイルパス.

public:
	CLog();
	~CLog();

	// インスタンスの取得.
	static CLog* GetInstance();

	// ログテキストファイルを作成.
	// アプリ起動時に作成する.
	static HRESULT OpenLogTextFile();

	// ログテキストファイルを閉じる.
	static HRESULT CloseLogTextFile();

	// ログの入力.
	static HRESULT Print( const char* message );

private:
	// コピー・ムーブコンストラクタ, 代入演算子の削除.
	CLog( const CLog & )				= delete;
	CLog& operator = ( const CLog & )	= delete;
	CLog( CLog && )						= delete;
	CLog& operator = ( CLog && )		= delete;
};

#endif	// #ifndef LOG_H.