#include "Common.h"

CCommon::CCommon()
	: m_pDevice11				( nullptr )
	, m_pContext11				( nullptr )
	, m_Tranceform				()
	, m_Color					( 1.0f, 1.0f, 1.0f, 1.0f )
{
}

CCommon::~CCommon()
{
	m_pContext11	= nullptr;
	m_pDevice11		= nullptr;
}

//--------------------------------------------.
// デバイスの取得、各情報の初期化.
//--------------------------------------------.
HRESULT CCommon::SetDevice( ID3D11DeviceContext* pContext11 )
{
	if( pContext11 == nullptr ){
		ERROR_MESSAGE( "デバイスコンテキスト取得 : 失敗" );
		return E_FAIL;
	}

	m_pContext11 = pContext11;
	m_pContext11->GetDevice( &m_pDevice11 );
	if( m_pDevice11 == nullptr ){
		ERROR_MESSAGE( "デバイス取得 : 失敗" );
		return E_FAIL;
	}

	return S_OK;
}

//--------------------------------------------.
// ブレンドを有効:無効に設定する.
//--------------------------------------------.
void CCommon::SetBlend( bool EnableAlpha )
{
	CDirectX11::SetBlend( EnableAlpha );
}

//--------------------------------------------.
// アルファカバレージを有効:無効に設定する.
//--------------------------------------------.
void CCommon::SetCoverage( bool EnableCoverage )
{
	CDirectX11::SetCoverage( EnableCoverage );
}

//--------------------------------------------.
// 深度テストを有効:無効に設定する.
//--------------------------------------------.
void CCommon::SetDeprh( bool flag )
{
	CDirectX11::SetDeprh( flag );
}

//--------------------------------------------.
// ラスタライザステート設定.
//--------------------------------------------.
void CCommon::SetRasterizerState( const ERS_STATE& rsState )
{
	CDirectX11::SetRasterizerState( rsState );
}
