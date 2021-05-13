#include "LoadManager.h"
#include "..\MeshResource\MeshResource.h"
#include "..\SpriteResource\SpriteResource.h"
#include "..\FontResource\FontResource.h"
#include "..\EffectResource\EffectResource.h"
#include "..\GameParamResource\GameParamResource.h"
#include "..\..\Utility\XAudio2\SoundManager.h"

CLoadManager::CLoadManager()
	: m_Thread				()
	, m_Mutex				()
	, m_isLoadEnd			( false )
	, m_isThreadJoined		( false )
	, m_isLoadFailed		( false )
{
}

CLoadManager::~CLoadManager()
{
	if( m_isLoadFailed == true ) m_Thread.join();
}

//------------------------.
// 読み込み.
//------------------------.
void CLoadManager::LoadResource(  
	HWND hWnd, 
	ID3D11Device* pDevice11, 
	ID3D11DeviceContext* pContext11, 
	LPDIRECT3DDEVICE9 pDevice9 )
{
	CFontResource::Load( pDevice11, pContext11 );
	CEffectResource::Load( pDevice11, pContext11 );
	CSoundManager::CreateSoundData();
	Sleep(100);
	auto load = [&]( 
		HWND hWnd, 
		ID3D11Device* pDevice11, 
		ID3D11DeviceContext* pContext11, 
		LPDIRECT3DDEVICE9 pDevice9 )
	{
		m_Mutex.lock();
		CLog::Print("------- スレッドロード開始 ------");
		Sleep(200);
		if( FAILED( CSpriteResource::Load( pContext11 ) )){
			m_Mutex.unlock();
			m_isLoadFailed = true;
			return;
		}
		Sleep(200);
		if( FAILED( CMeshResorce::Load( hWnd, pDevice11, pContext11, pDevice9 ) )){
			m_Mutex.unlock();
			m_isLoadFailed = true;
			return;
		}
		Sleep(200);
		CGameParamResource::ReadAllParam();
		Sleep(200);
		m_isLoadEnd = true;
		CLog::Print("------- スレッドロード終了 ------");
		m_Mutex.unlock();
	};
	m_Thread = std::thread( load, hWnd, pDevice11, pContext11, pDevice9 );
}

//------------------------.
// スレッドの解放.
//------------------------.
bool CLoadManager::ThreadRelease()
{
	if( m_isThreadJoined == true ) return true;
	if( m_isLoadEnd == false ) return false;
	DWORD code = -1;
	GetExitCodeThread( m_Thread.native_handle(),&code );
	if( code == 0xffffffff ){
		m_isThreadJoined = true;
	}
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
	return true;
}