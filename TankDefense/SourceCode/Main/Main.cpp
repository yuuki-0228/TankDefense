#include "Main.h"

#include <fstream>
#include <sstream>

#include "..\resource.h"
#include "..\Resource\LoadManager\LoadManager.h"	// ロードマネージャー.
#include "..\Scene\SceneManager\SceneManager.h"		// シーンマネージャー.
#include "..\Utility\FrameRate\FrameRate.h"			// フレームレート.
#include "..\Utility\FileManager\FileManager.h"		// ファイルマネージャー.
#include "..\Utility\ImGuiManager\ImGuiManager.h"	// ImGuiマネージャー.
#include "..\Utility\Input\Input.h"					// Input.
#include "..\Common\D3DX\D3DX9.h"					// DirectX11.
#include "..\Common\D3DX\D3DX11.h"					// DirectX9.
#include "..\Common\DebugText\DebugText.h"			// デバッグテキスト.
#include "..\Common\Font\Font.h"					// フォント描画.

#include "..\Object\CameraBase\CameraManager\CameraManager.h"	// カメラマネージャー.
#include "..\Object\LightBase\LightManager\LightManager.h"		// ライトマネージャー.
#include "..\Object\Collider\CollisionRender\CollisionRender.h"	// 当たり判定の描画.
#include "..\Object\Fade\Fade.h"

namespace
{
	constexpr char		WND_TITLE[]			= "Test";	// ウィンドウ名.
	constexpr char		APP_NAME[]			= "Test";	// アプリ名.

	constexpr float		FPS_RENDER_SIZE		= 0.5f;							// FPS描画サイズ.
	const D3DXVECTOR3	FPS_RENDER_POS		= { 0.0f, 1.0f, 0.0f };			// FPS描画位置.
	const D3DXVECTOR4	FPS_RENDER_COLOR	= { 0.9f, 0.2f, 0.2f, 1.0f };	// FPS描画色.
};

LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

CMain::CMain()
	: m_hWnd			( nullptr )
	, m_pLoadManager	( nullptr )
	, m_pFrameRate		( nullptr )
	, m_pSceneManager	( nullptr )
	, m_FPSRender		( nullptr )
#ifdef	_DEBUG
	, m_IsFPSRender		( true )
#else
	, m_IsFPSRender		( false )
#endif	// #ifdef _DEBUG.
{
	m_pLoadManager	= std::make_unique<CLoadManager>();
	m_pFrameRate	= std::make_unique<CFrameRate>( FPS );
	m_pSceneManager	= std::make_unique<CSceneManager>();
	m_FPSRender		= std::make_unique<CFont>();
}

CMain::~CMain()
{
}

//====================================.
//	初期化関数.
//====================================.
HRESULT CMain::Init()
{
	// DirectX9の構築.
	if( FAILED( CDirectX9::Create( m_hWnd ) )) return E_FAIL;
	// DirectX11の構築.
	if( FAILED( CDirectX11::Create( m_hWnd ) )) return E_FAIL;
	// ImGuiの初期化.
	if( FAILED( CImGuiManager::Init( m_hWnd, 
		CDirectX11::GetDevice(), 
		CDirectX11::GetContext() ))) return E_FAIL;

	// 画像描画クラスの初期化.
	if( FAILED( m_FPSRender->Init( CDirectX11::GetContext() ) ))		return E_FAIL;

	m_FPSRender->SetScale( FPS_RENDER_SIZE );
	m_FPSRender->SetColor( FPS_RENDER_COLOR );

	CInput::SetHWND( m_hWnd );

	return S_OK;
}

//====================================.
//	解放関数.
//====================================.
void CMain::Release()
{
	CImGuiManager::Release();
	CDirectX11::Release();
	CDirectX9::Release();
}

//====================================.
//	読み込み関数.
//====================================.
HRESULT CMain::Load()
{
	m_pLoadManager->LoadResource( m_hWnd, CDirectX11::GetDevice(), CDirectX11::GetContext(), CDirectX9::GetDevice() );

	if( FAILED( CDebugText::Init( CDirectX11::GetContext() ) ))	return E_FAIL;
	CFade::Init();

	return S_OK;
}

//====================================.
//	更新処理.
//====================================.
void CMain::Update()
{
	// デルタタイムの取得.
	const float	deltaTime = static_cast<float>(m_pFrameRate->GetDeltaTime());
	const bool	isLoadEnd = m_pLoadManager->ThreadRelease();

	CDebugText::PushText("------------------------------");
	CDebugText::PushText("L_Ctrl + F6 : EditImGuiRender");
	CDebugText::PushText("L_Ctrl + F7 : DebugRender");
	CDebugText::PushText("L_Ctrl + F8 : FPS Render");
	CDebugText::PushText("------------------------------");
	CDebugText::PushText("- CollisionRender Change Key -");
	CDebugText::PushText("------------------------------");
	CDebugText::PushText("L_Ctrl + F1 : SpherRender");
	CDebugText::PushText("L_Ctrl + F2 : CapsuleRender");
	CDebugText::PushText("L_Ctrl + F3 : BoxRender");
	CDebugText::PushText("L_Ctrl + F4 : RayRender");
	CDebugText::PushText("------------------------------");

	CInput::Update( deltaTime );

	CDirectX11::ClearBackBuffer();
	CDirectX11::SetBackBuffer();
	CImGuiManager::SetingNewFrame();

	m_pSceneManager->Init( isLoadEnd );					// シーンの初期化.
	m_pSceneManager->Update( deltaTime, isLoadEnd );	// シーンの更新.

	CFade::Update( deltaTime );	// フェードの更新.
	CFade::Render();			// フェードの描画.
	CDebugText::Render();		// デバッグテキストの描画.
	FPSRender();				// FPSの描画.

	CImGuiManager::Render();
	CDirectX11::SwapChainPresent();
}

//====================================.
//	メッセージループ.
//====================================.
void CMain::Loop()
{
	Load();	// 読み込み関数.

	// メッセージループ.
	MSG msg = { 0 };
	ZeroMemory( &msg, sizeof(msg) );

	while( msg.message != WM_QUIT )
	{
		if( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE )){
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		} else {
			// フレームレートの待機処理.
			if( m_pFrameRate->Wait() == true ) continue;
			Update();	// 更新処理.
		}
	}
}

//====================================.
// ウィンドウ初期化関数.
//====================================.
HRESULT CMain::InitWindow( HINSTANCE hInstance )
{
	// ウィンドウの定義.
	WNDCLASSEX wc = {};
	UINT windowStyle = CS_HREDRAW | CS_VREDRAW;

	wc.cbSize			= sizeof(wc);
	wc.style			= windowStyle;
	wc.lpfnWndProc		= WndProc;
	wc.hInstance		= hInstance;
	wc.hIcon			= LoadIcon( hInstance, MAKEINTRESOURCE(IDI_ICON) );
	wc.hCursor			= LoadCursor( nullptr, IDC_ARROW );
	wc.hbrBackground	= (HBRUSH)RGB(0, 0, 0);
	wc.lpszClassName	= APP_NAME;
	wc.hIconSm			= LoadIcon( hInstance, IDI_APPLICATION );

	// ウィンドウクラスをWindowsに登録.
	if( !RegisterClassEx( &wc ) ){
		ERROR_MESSAGE("ウィンドウクラス登録 : 失敗");
		return E_FAIL;
	}

	// ウィンドウ領域の調整.
	RECT rect = { 0, 0, WND_W, WND_H }; // 矩形構造体.
	DWORD dwStyle;	// ウィンドウスタイル.
	dwStyle		= WS_OVERLAPPEDWINDOW;

	if( AdjustWindowRect( &rect, dwStyle, FALSE ) == 0 ){
		ERROR_MESSAGE("ウィンドウ領域の調整 : 失敗");
		return E_FAIL;
	}
	RECT deskRect = {};	// 画面サイズ.
	SystemParametersInfo( SPI_GETWORKAREA, 0, &deskRect, 0 );
	int pos_x = 0, pos_y = 0;
	// 画面中央に配置できるように計算.
	pos_x = (((deskRect.right - deskRect.left) - (rect.right - rect.left)) / 2 + deskRect.left);
	pos_y = (((deskRect.bottom - deskRect.top) - (rect.bottom - rect.top)) / 2 + deskRect.top);

	
	// ウィンドウの作成.
	m_hWnd = CreateWindow(
		APP_NAME,				// アプリ名.
		WND_TITLE,				// ウィンドウタイトル.
		WS_OVERLAPPEDWINDOW,	// ウィンドウ種別(普通).
		pos_x, pos_y,			// 表示位置x,y座標.
		rect.right - rect.left,	// ウィンドウ幅.
		rect.bottom - rect.top,	// ウィンドウ高さ.
		nullptr,				// 親ウィンドウハンドル.
		nullptr,				// メニュー設定.
		hInstance,				// インスタンス番号.
		nullptr );				// ウィンドウ作成時に発生するイベントに渡すデータ.

	if( !m_hWnd ){
		ERROR_MESSAGE("ウィンドウ作成 : 失敗");
		return E_FAIL;
	} else {
		CLog::Print("ウィンドウ作成 : 成功");
	}

	// ウィンドウの表示.
	ShowWindow( m_hWnd, SW_SHOW );
	UpdateWindow( m_hWnd );

	return S_OK;
}

// FPSの描画.
void CMain::FPSRender()
{
	if( CKeyInput::IsPress(VK_LCONTROL) && CKeyInput::IsMomentPress(VK_F8) ) m_IsFPSRender = !m_IsFPSRender;

	if( m_IsFPSRender == false ) return;
	m_FPSRender->SetPosition( FPS_RENDER_POS );
	m_FPSRender->RenderUI( "    FPS    : " + std::to_string( (int)m_pFrameRate->GetFPS() ) );
	m_FPSRender->SetPosition( { 0.0f, m_FPSRender->FONT_SIZE*FPS_RENDER_SIZE, 0.0f } );
	m_FPSRender->RenderUI( " DeltaTime : " + std::to_string( m_pFrameRate->GetDeltaTime() ) );
}