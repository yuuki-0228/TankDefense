/**
* @file Effect.h.
* @brief Effekseer描画クラス.
* @author 福田玲也.
*/
#ifndef EFFECT_H
#define EFFECT_H

// 警告についてのコード分析を無効にする. 4005: 再定義.
#pragma warning(disable:4005)

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include <d3d11.h>
#pragma comment(lib,"d3d11.lib")



// XAudio2関係のコードを有効にする.
#if 0
#define ENABLE_XAUDIO2
#endif // #if 0.

#ifdef ENABLE_XAUDIO2
#include <XAudio2.h>
#endif	// #ifdef ENABLE_XAUDIO2

//-------------------------------------------------
//	ここからEffekseer関係 ヘッダ、ライブラリ読み込み.
//-------------------------------------------------
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>

#ifdef ENABLE_XAUDIO2
#include <EffekseerSoundXAudio2.h>
#endif	// #ifdef ENABLE_XAUDIO2

#if _DEBUG
#pragma comment(lib,"_Debug\\Effekseer.lib")
#pragma comment(lib,"_Debug\\EffekseerRendererDX11.lib")
#ifdef ENABLE_XAUDIO2
#pragma comment(lib,"_Debug\\EffekseerSoundXAudio2.lib")
#endif	// #ifdef ENABLE_XAUDIO2

#else	// #if _DEBUG
#pragma comment(lib,"_Release\\Effekseer.lib")
#pragma comment(lib,"_Release\\EffekseerRendererDX11.lib")
#ifdef ENABLE_XAUDIO2
#pragma comment(lib,"_Release\\EffekseerSoundXAudio2.lib")
#endif	//#ifdef ENABLE_XAUDIO2

#endif	// #if _DEBUG


#include <D3DX10.h>	// 注意:Effekseer関係より後で読み込むこと.
#pragma comment(lib, "d3dx10.lib")

/************************************************************
*	Effeksserのデータを使う為のクラス.
**/
class CEffect
{
public:
	typedef int Handle;
public:
	CEffect();	// コンストラクタ..
	CEffect( 
		ID3D11Device* pDevice11, 
		ID3D11DeviceContext* pContext11, 
		const std::string& fileName );
	~CEffect();	// デストラクタ.

	// 初期化.
	HRESULT Init( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11, const std::string& fileName );

	// 構築.
	HRESULT Create( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11 );
	// 破棄.
	void Destroy();
	// データ読み込み.
	HRESULT LoadData( const std::string& fileName );
	// データ解放.
	void ReleaseData();
	// 描画関数.
	void Render( const Effekseer::Handle& eHandle );

	// 再生関数.
	Effekseer::Handle Play( const D3DXVECTOR3& vPos ) const
	{
		return m_pManager->Play( m_pEffect, vPos.x, vPos.y, vPos.z );
	}
	// 停止関数.
	void Stop( const Effekseer::Handle& handle )
	{
		if( handle == -1 ) return; 
		m_pManager->StopEffect( handle );
	} 
	// 全て停止関数.
	void StopAll()
	{
		m_pManager->StopAllEffects();
	}
	// 一時停止もしくは再開する.
	void Pause( const Effekseer::Handle& handle, bool bFlag )
	{
		if( handle == -1 ) return;
		m_pManager->SetPaused( handle, bFlag );
	}
	// すべてを一時停止もしくは再開する.
	void AllPause( const bool& bFlag )
	{
		m_pManager->SetPausedToAllEffects( bFlag );
	};

	// 位置を指定する.
	void SetLocation( const Effekseer::Handle& handle, const D3DXVECTOR3& vPos )
	{
		if( handle == -1 ) return;
		m_pManager->SetLocation( handle, ConvertToVec3Efk(vPos) );
	}
	void SetLocation( const Effekseer::Handle& handle, const float& x, const float& y, const float& z )
	{
		if( handle == -1 ) return;
		m_pManager->SetLocation( handle, x, y, z );
	}
	// サイズを指定する.
	void SetScale( const Effekseer::Handle& handle, const D3DXVECTOR3& vScale )
	{
		if( handle == -1 ) return;
		m_pManager->SetScale( handle, vScale.x, vScale.y, vScale.z );
	}
	// 回転を指定する.
	void SetRotation( const Effekseer::Handle& handle, const D3DXVECTOR3& vRot )
	{
		if( handle == -1 ) return;
		m_pManager->SetRotation( handle, vRot.x, vRot.y, vRot.z );
	}
	// 回転を指定する(軸とラジアン).
	void SetRotation( const Effekseer::Handle& handle, const D3DXVECTOR3& vAxis, float fRadian )
	{
		if( handle == -1 ) return;
		m_pManager->SetRotation( handle, ConvertToVec3Efk(vAxis), fRadian );
	}
	// 再生速度を設定する.
	void SetSpeed( const Effekseer::Handle& handle, float fSpeed )
	{
		if( handle == -1 ) return;
		m_pManager->SetSpeed( handle, fSpeed );
	}
	// 存在しているか確認.
	bool IsExists( const Effekseer::Handle& handle )
	{
		if( handle == -1 ) return false;
		return m_pManager->Exists( handle );
	}
	// 描画しているか確認.
	bool IsShow( const Effekseer::Handle& handle )
	{
		if( handle == -1 ) return false;
		return m_pManager->GetShown( handle );
	}
	// エフェクトに使用されているインスタンス数を取得.
	int GetTotalInstanceCount()
	{
		return m_pManager->GetTotalInstanceCount();
	}

private:
	//--------------------------------------
	//	DirectX <--> Effekseer 変換関数.
	//--------------------------------------
	// vector変換.
	Effekseer::Vector3D	ConvertToVec3Efk( const D3DXVECTOR3& SrcVec3Dx ) const;
	D3DXVECTOR3 ConvertToVec3Dx( const Effekseer::Vector3D& SrcVec3Efk ) const;
	// matrix変換.
	Effekseer::Matrix44 ConvertToMatEfk( const D3DXMATRIX& SrcMatDx ) const;
	D3DXMATRIX ConvertToMatDx( const Effekseer::Matrix44& SrcMatEfk ) const;

	// ビュー行列を設定.
	void SetViewMatrix(D3DXMATRIX& mView);
	// プロジェクション行列を設定.
	void SetProjectionMatrix(D3DXMATRIX& mProj);

private:
	// エフェクトを動作させるために必要.
	static Effekseer::Manager*				m_pManager;
	static EffekseerRenderer::Renderer*		m_pRenderer;
	Effekseer::Effect*						m_pEffect;
#ifdef ENABLE_XAUDIO2
	// エフェクトに含まれる音の再生に必要.
	::EffekseerSound::Sound*		m_pSound;
	IXAudio2*						m_pXA2;
	IXAudio2MasteringVoice*			m_pXA2Master;
#endif//#ifdef ENABLE_XAUDIO2
};

#endif	// #ifndef EFFECT_H.