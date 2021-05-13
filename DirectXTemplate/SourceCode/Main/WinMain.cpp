#include "Main.h"
#include <Windows.h>

#include "..\Utility\ImGuiManager\ImGuiManager.h"
#include "..\Utility\Log\Log.h"
#include "..\Utility\Input\Input.h"
#include "..\Utility\XAudio2\SoundManager.h"

// ImGuiで使用.
extern LRESULT ImGui_ImplWin32_WndProcHandler( HWND, UINT, WPARAM, LPARAM );

std::unique_ptr<CMain> pMain = std::make_unique<CMain>();

int WINAPI WinMain( 
	HINSTANCE hInstance, 
	HINSTANCE hInstancePrev, 
	LPSTR pCmdLine, 
	int nCmdShow )
{
#ifdef _DEBUG
	// メモリリーク検出
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif	// #ifdef _DEBUG.

	CLog::OpenLogTextFile();
	
	if( pMain == nullptr ) return 0;

	// ウィンドウの作成.
	if( FAILED( pMain->InitWindow( hInstance ) )) return 0;
	// メイン初期化処理.
	if( FAILED( pMain->Init() )) return 0;

	// メインループ.
	pMain->Loop();

	// メイン解放処理.
	pMain->Release();
	
	CLog::CloseLogTextFile();

	ShowCursor( TRUE );	// マウスを表示する.

	return 0;
}

LRESULT CALLBACK WndProc(
	HWND hWnd, 
	UINT uMsg, 
	WPARAM wParam, 
	LPARAM lParam )
{
	if( ImGui_ImplWin32_WndProcHandler( hWnd, uMsg, wParam, lParam ) )
		return true;
	IMGUI_API
	switch( uMsg ){
	case WM_CREATE:
		break;

	case WM_KEYDOWN:
		// キー別の処理.
		switch( static_cast<char>(wParam) ){
		case VK_ESCAPE:	// ESCキー.
#ifdef _DEBUG
			if( MessageBox( nullptr, "ゲームを終了しますか？", "警告", MB_YESNO ) == IDYES ){ 
				PostQuitMessage(0); 
			}
			break;
#endif	// #ifdef _DEBUG.
		break;
		}
		break;
	case WM_MOUSEWHEEL:	// マウスホイールの入力.
		{
			const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
			CInput::SetMouseWheelDelta( delta );
		}
		break;
	case WM_CLOSE:
		CSoundManager::Release();
		DestroyWindow( hWnd );
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		break;
	}
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}