/**
* @file LoadManager.h.
* @brief 各リソース読み込みクラス.
* @author 福田玲也.
*/
#ifndef LOAD_MANAGER_H
#define LOAD_MANAGER_H

#include "..\..\Global.h"
#include <thread>
#include <mutex>

#include <memory>
#include <vector>

/*********************************
*	各リソース類の読み込みをまとめるクラス.
*		Load画像などの表示をここに入れてもいいかも.
*/
class CLoadManager
{
public:
	CLoadManager();
	~CLoadManager();

	// 読み込み.
	void LoadResource( 
		HWND hWnd, 
		ID3D11Device* pDevice11, 
		ID3D11DeviceContext* pContext11, 
		LPDIRECT3DDEVICE9 pDevice9  );
	// スレッドの解放.
	bool ThreadRelease();

	// ロード失敗.
	inline bool IsLoadFailed() const { return m_isLoadFailed; }
	// ロードが終了したか.
	inline bool	IsLoadEnd() const { return m_isThreadJoined; }

private:
	std::thread m_Thread;		// スレッド.
	std::mutex	m_Mutex;
	bool m_isLoadEnd;			// ロードが終了したか.
	bool m_isThreadJoined;		// スレッドが解放されたか.
	bool m_isLoadFailed;		// ロード失敗.
};

#endif	// #ifndef LOAD_MANAGER_H.