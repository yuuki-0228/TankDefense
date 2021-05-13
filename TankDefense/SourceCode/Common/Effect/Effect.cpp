#include "Effect.h"
#include "..\..\Object\CameraBase\CameraManager\CameraManager.h"
#include <thread>
#include <mutex>

// 定数宣言.
// 描画用インスタンス(スプライト)最大数.
const int RENDER_SPRITE_MAX = 20000;
// エフェクト管理用インスタンス最大数.
const int EFFECT_INSTANCE_MAX = 10000;


Effekseer::Manager*				CEffect::m_pManager = nullptr;
EffekseerRenderer::Renderer*	CEffect::m_pRenderer = nullptr;


CEffect::CEffect()
	: m_pEffect		( nullptr )
#ifdef ENABLE_XAUDIO2
	, m_pSound(nullptr)
	, m_pXA2(nullptr)
	, m_pXA2Master(nullptr)
#endif//#ifdef ENABLE_XAUDIO2
{
}

CEffect::CEffect( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11, const std::string& fileName )
	: CEffect	()
{
	if( FAILED( Init( pDevice11, pContext11,  fileName )) ){
		ERROR_MESSAGE("Effekseer初期化失敗");
	}
}

CEffect::~CEffect()
{
	Destroy();
}

//-----------------------------------------------------------.
// 初期化.
//-----------------------------------------------------------.
HRESULT CEffect::Init( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11, const std::string& fileName )
{
	if( FAILED( Create( pDevice11, pContext11 )) ){
		ERROR_MESSAGE("Effekseer構築失敗");
		return E_FAIL;
	}
	if( FAILED( LoadData( fileName )) ){
		ERROR_MESSAGE("Effekseer読み込み失敗");
		return E_FAIL;
	}

	std::string msg = fileName + " 読み込み : 成功";
	CLog::Print( msg.c_str() );
	return S_OK;
}

//-----------------------------------------------------------.
// 構築.
//-----------------------------------------------------------.
HRESULT CEffect::Create( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11 )
{
	if( m_pManager != nullptr ) return S_OK;
	std::mutex	mutex;
	mutex.lock();
#ifdef ENABLE_XAUDIO2
	// XAudio2の初期化を行う.
	if (FAILED(
		XAudio2Create(&m_pXA2)))
	{
		_ASSERT_EXPR(false, "XAudio2作成失敗");
		return E_FAIL;
	}
	if (FAILED(
		m_pXA2->CreateMasteringVoice(&m_pXA2Master)))
	{
		_ASSERT_EXPR(false, "MasteringVoice作成失敗");
		return E_FAIL;
	}
#endif	// #ifdef ENABLE_XAUDIO2

	// 描画用インスタンスの生成.
	m_pRenderer
		= ::EffekseerRendererDX11::Renderer::Create(
			pDevice11, pContext11, RENDER_SPRITE_MAX );
	// エフェクト管理用インスタンスの生成.
	m_pManager = ::Effekseer::Manager::Create( EFFECT_INSTANCE_MAX );

	// 描画用インスタンスから描画機能を設定.
	m_pManager->SetSpriteRenderer( m_pRenderer->CreateSpriteRenderer() );
	m_pManager->SetRibbonRenderer( m_pRenderer->CreateRibbonRenderer() );
	m_pManager->SetRingRenderer( m_pRenderer->CreateRingRenderer() );
	m_pManager->SetModelRenderer( m_pRenderer->CreateModelRenderer() );
	m_pManager->SetTrackRenderer( m_pRenderer->CreateTrackRenderer() );

	// 描画用インスタンスからテクスチャの読み込み機能を設定.
	// 独自拡張可能、現在はファイルから読み込んでいる.
	m_pManager->SetTextureLoader( m_pRenderer->CreateTextureLoader() );
	m_pManager->SetModelLoader( m_pRenderer->CreateModelLoader() );
	m_pManager->SetMaterialLoader( m_pRenderer->CreateMaterialLoader() );

#ifdef ENABLE_XAUDIO2
	// 音の再生用インスタンスの生成.
	m_pSound = ::EffekseerSound::Sound::Create(m_pXA2, 16, 16);

	// 音の再生用インスタンスから再生機能を設定.
	m_pManager->SetSoundPlayer(m_pSound->CreateSoundPlayer());

	// 音の再生用インスタンスからサウンドデータの読み込み機能を設定.
	// 独自拡張可能、現在はファイルから読み込んでいる.
	m_pManager->SetSoundLoader(m_pSound->CreateSoundLoader());
#endif//#ifdef ENABLE_XAUDIO2
	mutex.unlock();
	return S_OK;
}

//-----------------------------------------------------------.
// 破棄.
//-----------------------------------------------------------.
void CEffect::Destroy()
{
	// エフェクトデータの解放.
	ReleaseData();

	// 先にエフェクト管理用インスタンスを破棄.
	if( m_pManager != nullptr ){
		m_pManager->Destroy();
		m_pManager = nullptr;
	}


#ifdef ENABLE_XAUDIO2
	// 次に音の再生用インスタンスを破棄.
	m_pSound->Destroy();
#endif//#ifdef ENABLE_XAUDIO2

	// 次に描画用インスタンスを破棄.
	if( m_pRenderer != nullptr ){
		m_pRenderer->Destroy();
		m_pRenderer = nullptr;
	}

#ifdef ENABLE_XAUDIO2
	// XAudio2の解放.
	if (m_pXA2Master != nullptr) {
		m_pXA2Master->DestroyVoice();
		m_pXA2Master = nullptr;
	}
	if( m_pXA2 != nullptr ){
		m_pXA2->Release();
		m_pXA2 = nullptr;
	}
#endif//#ifdef ENABLE_XAUDIO2
}

//-----------------------------------------------------------.
// データ読み込み.
//-----------------------------------------------------------.
HRESULT CEffect::LoadData( const std::string& fileName )
{
	std::mutex	mutex;
	mutex.lock();
	// 文字変換.
	const size_t charSize = fileName.length() + 1;	// 入力文字のサイズ+1を取得.
	wchar_t* FileName = nullptr;
	FileName = new wchar_t[charSize];				// wchar_t型を"charSize"分確保.
	size_t ret;										// 変換後のwchar_tのサイズ取得用.
	mbstowcs_s( &ret, FileName, charSize, fileName.c_str(), _TRUNCATE );

	// エフェクトの読み込み.
	m_pEffect = Effekseer::Effect::Create( m_pManager, (const EFK_CHAR*)FileName );
	if( m_pEffect == nullptr ){
		_ASSERT_EXPR(false, L"ｴﾌｪｸﾄ読み込み失敗");
		return E_FAIL;
	}

	delete[] FileName;

	mutex.unlock();
	return S_OK;
}

//-----------------------------------------------------------.
// データ解放.
//-----------------------------------------------------------.
void CEffect::ReleaseData()
{
	// エフェクトの解放.
	if( m_pEffect != nullptr ){
		m_pEffect->Release();
		m_pEffect = nullptr;
	}
}

//-----------------------------------------------------------.
// 描画関数.
//-----------------------------------------------------------.
void CEffect::Render( const Effekseer::Handle& eHandle )
{
	if( eHandle == -1 ) return;

	D3DXMATRIX view = CCameraManager::GetViewMatrix();
	D3DXMATRIX Proj = CCameraManager::GetProjMatrix();

	// ビュー行列を設定.
	SetViewMatrix(view);

	// プロジェクション行列を設定.
	SetProjectionMatrix(Proj);

	// エフェクトの更新処理.
	m_pManager->BeginUpdate();
	m_pManager->UpdateHandle( eHandle );
	m_pManager->EndUpdate();
	// エフェクトの描画開始処理.
	m_pRenderer->BeginRendering();
	m_pManager->DrawHandle( eHandle );
	m_pRenderer->EndRendering();
}

//-----------------------------------------------------------.

// DirectX D3DXVECTOR3 -> Effekseer Vector3 に変換する.
//-----------------------------------------------------------.
Effekseer::Vector3D	CEffect::ConvertToVec3Efk( const D3DXVECTOR3& SrcVec3Dx ) const
{
	return Effekseer::Vector3D(
		SrcVec3Dx.x, SrcVec3Dx.y, SrcVec3Dx.z);
}

//-----------------------------------------------------------.
// Effekseer Vector3 -> DirectX D3DXVECTOR3 に変換する.
//-----------------------------------------------------------.
D3DXVECTOR3 CEffect::ConvertToVec3Dx( const Effekseer::Vector3D& SrcVec3Efk) const
{
	return D3DXVECTOR3(SrcVec3Efk.X, SrcVec3Efk.Y, SrcVec3Efk.Z);
}

//-----------------------------------------------------------.
// DirectX D3DXMATRIX -> Effekseer Matrix44 に変換する.
//-----------------------------------------------------------.
Effekseer::Matrix44 CEffect::ConvertToMatEfk( const D3DXMATRIX & SrcMatDx) const
{
	Effekseer::Matrix44 DestMatEfk;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			DestMatEfk.Values[i][j] = SrcMatDx.m[i][j];
		}
	}

	return DestMatEfk;
}

//-----------------------------------------------------------.
// Effekseer Matrix44 -> DirectX D3DXMATRIX に変換する.
//-----------------------------------------------------------.
D3DXMATRIX CEffect::ConvertToMatDx( const Effekseer::Matrix44& SrcMatEfk ) const
{
	D3DXMATRIX DestMatDx;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			DestMatDx.m[i][j] = SrcMatEfk.Values[i][j];
		}
	}

	return DestMatDx;
}

//-----------------------------------------------------------.
// ビュー行列を設定.
//-----------------------------------------------------------.
void CEffect::SetViewMatrix(D3DXMATRIX& mView)
{
	Effekseer::Matrix44 mViewEfk = ConvertToMatEfk(mView);

	// カメラ行列を設定.
	m_pRenderer->SetCameraMatrix(mViewEfk);
}

//-----------------------------------------------------------.
// プロジェクション行列を設定.
//-----------------------------------------------------------.
void CEffect::SetProjectionMatrix(D3DXMATRIX& mProj)
{
	Effekseer::Matrix44 mProjEfk = ConvertToMatEfk(mProj);

	// プロジェクション行列を設定.
	m_pRenderer->SetProjectionMatrix(mProjEfk);
}
