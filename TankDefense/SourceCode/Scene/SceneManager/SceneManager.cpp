#include "SceneManager.h"
#include "..\Scenes\SceneList.h"
#include "..\..\Utility\Input\Input.h"
#include "..\..\Common\RenderingTextuer\RenderingTextuerManager\RenderingTextuerManager.h"
#include "..\..\Object\Collider\CollisionRender\CollisionRender.h"
#include "..\..\Object\LoadRender\LoadRender.h"
#include "..\..\Object\CameraBase\CameraManager\CameraManager.h"
#include "..\..\Object\LightBase\LightManager\LightManager.h"
#include "..\..\Object\Fade\Fade.h"

CSceneManager::CSceneManager()
	: m_hWnd					( nullptr )
	, m_pScene					( nullptr )
	, m_pLoadRender				( nullptr )
	, m_pRenderingTexManager	( nullptr )
	, m_RenderFunction			( nullptr )
	, m_NowScene				( EScene::Start )
	, m_NextScene				( EScene::Start )
	, m_IsThreadLoadEnd			( false )
	, m_IsLoadEnd				( false )
	, m_IsOldLoadEnd			( false )
	, m_OnceStep				( false )
	, m_isThreadJoined			( false )
{
	m_pLoadRender = std::make_unique<CLoadRender>();
	m_pRenderingTexManager = std::make_unique<CRenderingTexterManager>();
	m_RenderFunction = [&]()
	{ 
		m_pScene->ModelRender();
		CLightManager::PositionRender();		// ライトの位置描画.
		CCollisionRender::Render();				// 当たり判定の描画.
	};
}

CSceneManager::~CSceneManager()
{
}

//=================================.
// 初期化.
//=================================.
void CSceneManager::Init( const bool& isResourceLoadEnd )
{
	if( isResourceLoadEnd == false ) return;
	if( m_OnceStep == true ) return;
	if( m_pRenderingTexManager->Init( CDirectX11::GetContext() ) ) return;
	ChangeNextScene();
	m_OnceStep = true;
}

//=================================.
//	更新関数.
//=================================.
void CSceneManager::Update( const float& deltaTime, const bool& isResourceLoadEnd )
{
	// リソースの読み込みが終了してない場合,
	//	ロード用の描画をする.
	LoadRender( deltaTime, isResourceLoadEnd );
	if( m_pScene == nullptr ) return;

	// スレッドの解放する.
	ThreadRelease();

	if( m_IsLoadEnd == false ){
		// スレッドのロードが終わって、フェードが終了(行ってない)している場合,
		if( m_IsThreadLoadEnd == true && CFade::IsFadeEnd() == true ){
			CFade::KeepRender();
			CFade::SetFadeIn( true );	// フェードイン.
		}
		// スレッドのロードが終わって、フェード処理がOutの場合,
		//	ロード終了フラグを立てて、シーンの更新を行う.
		if( m_IsThreadLoadEnd == true && CFade::IsFadeOut() == true ){
			m_IsLoadEnd = true;
		}
		// ロードの描画.
		LoadRender( deltaTime, false );
	} else {

		// シーンの更新.
		SceneUpdate( deltaTime );
	}
}

//=================================.
// 次のシーンに変更.
//=================================.
void CSceneManager::ChangeNextScene()
{
	switch( m_NextScene )
	{
	case EScene::Title:
		m_pScene	= std::make_shared<CTitle>( this );
		m_NowScene	= m_NextScene;
		m_NextScene	= EScene::GameMain;
		break;
	case EScene::GameMain:
		m_pScene	= std::make_shared<CGame>( this );
		m_NowScene	= m_NextScene;
		m_NextScene	= EScene::GameMain;
		break;
	default:
		return;
	}

	// 一回目以降はフェードアウトを行う.
	if( m_OnceStep == true ){
		CFade::KeepRender( false );
		CFade::SetFadeOut();
	}

	// 各フラグを下ろす.
	m_isThreadJoined	= false;
	m_IsThreadLoadEnd	= false;
	m_IsLoadEnd			= false;

	// ロード関数をスレッドに渡す.
	m_Thread = std::thread( 
		[&]()
		{
			// スレッドのロック.
			std::lock_guard<std::mutex> lock(m_Mutex);
			// 念のため少しスリープして処理を遅らせる.
			Sleep( 1000 );
			m_IsThreadLoadEnd = m_pScene->Load();
		});
}

//=================================.
// 読み込み時の描画.
//=================================.
void CSceneManager::LoadRender( const float& deltaTime, const bool& isLoad )
{
	if( isLoad == true || m_IsLoadEnd == true ) return;
	m_pLoadRender->Init();
	m_pLoadRender->Update( deltaTime );
	m_pLoadRender->Render();
}

//=================================.
// シーンの更新.
//=================================.
void CSceneManager::SceneUpdate( const float& deltaTime )
{
	m_pScene->Update();
	m_pScene->ChangeScene();

	// 上の更新で、シーンが変更しているかもしれないので,
	//	ロードの確認をする.
	if( m_IsThreadLoadEnd == false ) return;

	CCameraManager::Update( deltaTime );
	CLightManager::Update( deltaTime );

	m_pRenderingTexManager->Render( m_RenderFunction );
	m_pScene->SpriteRender();
}

// スレッドの解放.
void CSceneManager::ThreadRelease()
{
	if( m_isThreadJoined == true ) return;
	if( m_IsThreadLoadEnd == true ){
		DWORD code = -1;
		GetExitCodeThread( m_Thread.native_handle(),&code );
		if( code == 0xffffffff ) m_isThreadJoined = true;
		if( code == 0 ){
			m_Thread.join();
			while (1)
			{
				if( m_Thread.joinable() != true ){
					m_isThreadJoined = true;
					break;
				}
			}
		}
	}
}