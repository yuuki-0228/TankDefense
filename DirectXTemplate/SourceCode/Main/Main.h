#ifndef MAIN_H
#define MAIN_H

#include <memory>
#include <string>

#include "..\Global.h"

class CFrameRate;
class CSceneManager;
class CSprite;
class CFont;
class CLoadManager;

/**************************************************
*	メインクラス.
**/
class CMain
{
public:
	CMain();
	~CMain();
	// ウィンドウ初期化関数.
	HRESULT InitWindow( HINSTANCE hInstance );

	// メッセージループ.
	void Loop();

	// 初期化関数.
	HRESULT Init();
	// 解放関数.
	void Release();

private:
	// 更新処理.
	void Update();

	// 読み込み関数.
	HRESULT Load();

	// FPSの描画.
	void FPSRender();

private:
	HWND m_hWnd;
	std::unique_ptr<CLoadManager>	m_pLoadManager;		// ロードマネージャー.
	std::unique_ptr<CFrameRate>		m_pFrameRate;		// フレームレート.
	std::unique_ptr<CSceneManager>	m_pSceneManager;	// シーンマネージャー.
	std::unique_ptr<CFont>			m_FPSRender;		// FPSの描画.

	bool	m_IsFPSRender;	// FPSを描画するか.
};

#endif	// #ifndef MAIN_H.
