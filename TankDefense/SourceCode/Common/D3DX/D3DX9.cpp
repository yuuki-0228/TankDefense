#include "D3DX9.h"

CDirectX9::CDirectX9()
	: m_hWnd		( nullptr )
	, m_pDevice9	( nullptr )
{
}

CDirectX9::~CDirectX9()
{
}

//-----------------------------------.
// インスタンスの取得.
//-----------------------------------.
CDirectX9* CDirectX9::GetInstance()
{
	static std::unique_ptr<CDirectX9> pInstance = 
		std::make_unique<CDirectX9>();
	return pInstance.get();
}

//-----------------------------------.
// DirectX9の構築.
//-----------------------------------.
HRESULT CDirectX9::Create( HWND hWnd )
{
	GetInstance()->m_hWnd = hWnd;

	if( FAILED( GetInstance()->CreateDevice9() )) return E_FAIL;
	CLog::Print( "DIrectX9デバイス作成 : 成功" );

	return S_OK;
}

//-----------------------------------.
// DirectX9の解放.
//-----------------------------------.
HRESULT CDirectX9::Release()
{
	SAFE_RELEASE( GetInstance()->m_pDevice9 );
	GetInstance()->m_hWnd = nullptr;

	return S_OK;
}

//-----------------------------------.
// デバイス9の作成.
//-----------------------------------.
HRESULT CDirectX9::CreateDevice9()
{
	LPDIRECT3D9 m_pD3d9 = nullptr; // Dx9オブジェクト.

	//「Direct3D」オブジェクトの作成.
	m_pD3d9 = Direct3DCreate9( D3D_SDK_VERSION );
	if( m_pD3d9 == nullptr ){
		ERROR_MESSAGE("Dx9オブジェクト作成 : 失敗");
		return E_FAIL;
	}

	// Diret3D デバイスオブジェクトの作成.
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof( d3dpp ) );
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;	// バックバッファのフォーマット.
	d3dpp.BackBufferCount = 1;					// バックバッファの数.
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;	// スワップエフェクト.
	d3dpp.Windowed = true;						// ウィンドウモード.
	d3dpp.EnableAutoDepthStencil = true;		// ステンシル有効.
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;	// ステンシルのフォーマット(16bit).

	auto createDevice = [&]( const D3DDEVTYPE& deviceType, const DWORD& behaviorFlags )
	{
		return m_pD3d9->CreateDevice(
			D3DADAPTER_DEFAULT, 
			deviceType, 
			m_hWnd,
			behaviorFlags,
			&d3dpp, 
			&m_pDevice9 );
	};

	// デバイス作成
	// (HALﾓｰﾄﾞ:描画と頂点処理をGPUで行う).
	if( SUCCEEDED( createDevice( D3DDEVTYPE_HAL, D3DCREATE_HARDWARE_VERTEXPROCESSING ) )){
		SAFE_RELEASE( m_pD3d9 );
		return S_OK;
	}

	// (HALﾓｰﾄﾞ:描画はGPU、頂点処理をCPUで行う).
	if( SUCCEEDED( createDevice( D3DDEVTYPE_HAL, D3DCREATE_SOFTWARE_VERTEXPROCESSING ) )){
		SAFE_RELEASE( m_pD3d9 );
		return S_OK;
	}
	MessageBox( nullptr, "HALﾓｰﾄﾞでﾃﾞﾊﾞｲｽ作成できません\nREFﾓｰﾄﾞで再試行します", "警告", MB_OK );

	// (REFﾓｰﾄﾞ:描画はCPU、頂点処理をGPUで行う).
	if( SUCCEEDED( createDevice( D3DDEVTYPE_REF, D3DCREATE_HARDWARE_VERTEXPROCESSING ) )){
		SAFE_RELEASE( m_pD3d9 );
		return S_OK;
	}

	// (REFﾓｰﾄﾞ:描画と頂点処理をCPUで行う).
	if( SUCCEEDED( createDevice( D3DDEVTYPE_REF, D3DCREATE_SOFTWARE_VERTEXPROCESSING ) )){
		SAFE_RELEASE( m_pD3d9 );
		return S_OK;
	}

	ERROR_MESSAGE("Direct3デバイス作成 : 失敗");

	SAFE_RELEASE( m_pD3d9 );
	return E_FAIL;
}