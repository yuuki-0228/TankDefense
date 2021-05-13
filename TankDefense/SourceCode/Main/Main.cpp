#include "Main.h"

#include <fstream>
#include <sstream>

#include "..\resource.h"
#include "..\Resource\LoadManager\LoadManager.h"	// ���[�h�}�l�[�W���[.
#include "..\Scene\SceneManager\SceneManager.h"		// �V�[���}�l�[�W���[.
#include "..\Utility\FrameRate\FrameRate.h"			// �t���[�����[�g.
#include "..\Utility\FileManager\FileManager.h"		// �t�@�C���}�l�[�W���[.
#include "..\Utility\ImGuiManager\ImGuiManager.h"	// ImGui�}�l�[�W���[.
#include "..\Utility\Input\Input.h"					// Input.
#include "..\Common\D3DX\D3DX9.h"					// DirectX11.
#include "..\Common\D3DX\D3DX11.h"					// DirectX9.
#include "..\Common\DebugText\DebugText.h"			// �f�o�b�O�e�L�X�g.
#include "..\Common\Font\Font.h"					// �t�H���g�`��.

#include "..\Object\CameraBase\CameraManager\CameraManager.h"	// �J�����}�l�[�W���[.
#include "..\Object\LightBase\LightManager\LightManager.h"		// ���C�g�}�l�[�W���[.
#include "..\Object\Collider\CollisionRender\CollisionRender.h"	// �����蔻��̕`��.
#include "..\Object\Fade\Fade.h"

namespace
{
	constexpr char		WND_TITLE[]			= "Test";	// �E�B���h�E��.
	constexpr char		APP_NAME[]			= "Test";	// �A�v����.

	constexpr float		FPS_RENDER_SIZE		= 0.5f;							// FPS�`��T�C�Y.
	const D3DXVECTOR3	FPS_RENDER_POS		= { 0.0f, 1.0f, 0.0f };			// FPS�`��ʒu.
	const D3DXVECTOR4	FPS_RENDER_COLOR	= { 0.9f, 0.2f, 0.2f, 1.0f };	// FPS�`��F.
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
//	�������֐�.
//====================================.
HRESULT CMain::Init()
{
	// DirectX9�̍\�z.
	if( FAILED( CDirectX9::Create( m_hWnd ) )) return E_FAIL;
	// DirectX11�̍\�z.
	if( FAILED( CDirectX11::Create( m_hWnd ) )) return E_FAIL;
	// ImGui�̏�����.
	if( FAILED( CImGuiManager::Init( m_hWnd, 
		CDirectX11::GetDevice(), 
		CDirectX11::GetContext() ))) return E_FAIL;

	// �摜�`��N���X�̏�����.
	if( FAILED( m_FPSRender->Init( CDirectX11::GetContext() ) ))		return E_FAIL;

	m_FPSRender->SetScale( FPS_RENDER_SIZE );
	m_FPSRender->SetColor( FPS_RENDER_COLOR );

	CInput::SetHWND( m_hWnd );

	return S_OK;
}

//====================================.
//	����֐�.
//====================================.
void CMain::Release()
{
	CImGuiManager::Release();
	CDirectX11::Release();
	CDirectX9::Release();
}

//====================================.
//	�ǂݍ��݊֐�.
//====================================.
HRESULT CMain::Load()
{
	m_pLoadManager->LoadResource( m_hWnd, CDirectX11::GetDevice(), CDirectX11::GetContext(), CDirectX9::GetDevice() );

	if( FAILED( CDebugText::Init( CDirectX11::GetContext() ) ))	return E_FAIL;
	CFade::Init();

	return S_OK;
}

//====================================.
//	�X�V����.
//====================================.
void CMain::Update()
{
	// �f���^�^�C���̎擾.
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

	m_pSceneManager->Init( isLoadEnd );					// �V�[���̏�����.
	m_pSceneManager->Update( deltaTime, isLoadEnd );	// �V�[���̍X�V.

	CFade::Update( deltaTime );	// �t�F�[�h�̍X�V.
	CFade::Render();			// �t�F�[�h�̕`��.
	CDebugText::Render();		// �f�o�b�O�e�L�X�g�̕`��.
	FPSRender();				// FPS�̕`��.

	CImGuiManager::Render();
	CDirectX11::SwapChainPresent();
}

//====================================.
//	���b�Z�[�W���[�v.
//====================================.
void CMain::Loop()
{
	Load();	// �ǂݍ��݊֐�.

	// ���b�Z�[�W���[�v.
	MSG msg = { 0 };
	ZeroMemory( &msg, sizeof(msg) );

	while( msg.message != WM_QUIT )
	{
		if( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE )){
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		} else {
			// �t���[�����[�g�̑ҋ@����.
			if( m_pFrameRate->Wait() == true ) continue;
			Update();	// �X�V����.
		}
	}
}

//====================================.
// �E�B���h�E�������֐�.
//====================================.
HRESULT CMain::InitWindow( HINSTANCE hInstance )
{
	// �E�B���h�E�̒�`.
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

	// �E�B���h�E�N���X��Windows�ɓo�^.
	if( !RegisterClassEx( &wc ) ){
		ERROR_MESSAGE("�E�B���h�E�N���X�o�^ : ���s");
		return E_FAIL;
	}

	// �E�B���h�E�̈�̒���.
	RECT rect = { 0, 0, WND_W, WND_H }; // ��`�\����.
	DWORD dwStyle;	// �E�B���h�E�X�^�C��.
	dwStyle		= WS_OVERLAPPEDWINDOW;

	if( AdjustWindowRect( &rect, dwStyle, FALSE ) == 0 ){
		ERROR_MESSAGE("�E�B���h�E�̈�̒��� : ���s");
		return E_FAIL;
	}
	RECT deskRect = {};	// ��ʃT�C�Y.
	SystemParametersInfo( SPI_GETWORKAREA, 0, &deskRect, 0 );
	int pos_x = 0, pos_y = 0;
	// ��ʒ����ɔz�u�ł���悤�Ɍv�Z.
	pos_x = (((deskRect.right - deskRect.left) - (rect.right - rect.left)) / 2 + deskRect.left);
	pos_y = (((deskRect.bottom - deskRect.top) - (rect.bottom - rect.top)) / 2 + deskRect.top);

	
	// �E�B���h�E�̍쐬.
	m_hWnd = CreateWindow(
		APP_NAME,				// �A�v����.
		WND_TITLE,				// �E�B���h�E�^�C�g��.
		WS_OVERLAPPEDWINDOW,	// �E�B���h�E���(����).
		pos_x, pos_y,			// �\���ʒux,y���W.
		rect.right - rect.left,	// �E�B���h�E��.
		rect.bottom - rect.top,	// �E�B���h�E����.
		nullptr,				// �e�E�B���h�E�n���h��.
		nullptr,				// ���j���[�ݒ�.
		hInstance,				// �C���X�^���X�ԍ�.
		nullptr );				// �E�B���h�E�쐬���ɔ�������C�x���g�ɓn���f�[�^.

	if( !m_hWnd ){
		ERROR_MESSAGE("�E�B���h�E�쐬 : ���s");
		return E_FAIL;
	} else {
		CLog::Print("�E�B���h�E�쐬 : ����");
	}

	// �E�B���h�E�̕\��.
	ShowWindow( m_hWnd, SW_SHOW );
	UpdateWindow( m_hWnd );

	return S_OK;
}

// FPS�̕`��.
void CMain::FPSRender()
{
	if( CKeyInput::IsPress(VK_LCONTROL) && CKeyInput::IsMomentPress(VK_F8) ) m_IsFPSRender = !m_IsFPSRender;

	if( m_IsFPSRender == false ) return;
	m_FPSRender->SetPosition( FPS_RENDER_POS );
	m_FPSRender->RenderUI( "    FPS    : " + std::to_string( (int)m_pFrameRate->GetFPS() ) );
	m_FPSRender->SetPosition( { 0.0f, m_FPSRender->FONT_SIZE*FPS_RENDER_SIZE, 0.0f } );
	m_FPSRender->RenderUI( " DeltaTime : " + std::to_string( m_pFrameRate->GetDeltaTime() ) );
}