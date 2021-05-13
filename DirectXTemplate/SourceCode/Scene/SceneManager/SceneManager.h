/**
* @file SceneManager.h.
* @brief シーン管理クラス.
* @author 福田玲也.
*/
#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "..\SceneBase\SceneBase.h"
#include <thread>
#include <mutex>

class CSpriteRender;
class CLoadRender;
class CRenderingTexterManager;

// シーンの種類.
enum class enSceneNo : unsigned char;
using ESceneNo	= enSceneNo;
using EScene	= enSceneNo;

/**********************************
*	シーン管理クラス.
*/
class CSceneManager
{	
public:
	CSceneManager();
	~CSceneManager();

	// 初期化.
	void Init( const bool& isResourceLoadEnd );

	// 更新関数.
	void Update( const float& deltaTime, const bool& isResourceLoadEnd );

	// 次のシーンに変更.
	void ChangeNextScene();
private:
	// 読み込み時の描画.
	void LoadRender( const float& deltaTime, const bool& isLoad );
	// シーンの更新.
	void SceneUpdate( const float& deltaTime );
	// スレッドの解放.
	void ThreadRelease();
private:
	HWND							m_hWnd;			// ウィンドウハンドル.
	std::shared_ptr<CSceneBase>		m_pScene;		// シーンクラス.
	std::unique_ptr<CLoadRender>	m_pLoadRender;	// 読み込みの描画.
	std::unique_ptr<CRenderingTexterManager>	m_pRenderingTexManager;
	std::function<void()>			m_RenderFunction;
	EScene							m_NowScene;		// 現在のシーン.
	EScene							m_NextScene;	// 次のシーン.
	bool							m_IsThreadLoadEnd;	// ロードが終了したか.
	bool							m_IsLoadEnd;
	bool							m_IsOldLoadEnd;	// ロードが終了したか.
	bool							m_OnceStep;		// 一回だけ動作.
	std::thread m_Thread;		// スレッド.
	std::mutex	m_Mutex;
	bool m_isThreadJoined;
};

#endif // #ifndef SCENE_MANAGER_H.