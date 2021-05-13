/**
* @file D3DX11.h.
* @brief DirectX11デバイスクラス.
* @author 福田玲也.
*/
#ifndef D3DX11_H
#define D3DX11_H

#include "..\..\Global.h"
#include <vector>

// ラスタライザステート.
enum class enRS_STATE
{
	None,	// 正背面描画.
	Back,	// 背面を描画しない.
	Front,	// 正面を描画しない.
	Wire,	// ワイヤーフレーム描画.

	Max,
} typedef ERS_STATE;

/*********************************
*	DirectX11.
**/
class CDirectX11
{
public: //----- 関数 ------.
	CDirectX11();
	~CDirectX11();

	// インスタンスの取得.
	static CDirectX11* GetInstance();

	// DirectX11構築関数.
	static HRESULT Create( HWND hWnd );
	// DirectX11解放関数.
	static HRESULT Release();

	// クリアバックバッファ.
	static void ClearBackBuffer();
	// スワップチェーンプレゼント.
	static void SwapChainPresent();

	// デバイス11の取得関数.
	static ID3D11Device* GetDevice() { return GetInstance()->m_pDevice11; }
	// コンテキスト11の取得関数.
	static ID3D11DeviceContext* GetContext() { return GetInstance()->m_pContext11; }
	// デプスステンシルビューの取得.
	static ID3D11DepthStencilView* GetDepthSV(){ return GetInstance()->m_pBackBuffer_DSTexDSV; }
	// ウィンドウサイズの取得.
	static UINT GetWndWidth(){ return GetInstance()->m_WndWidth; }
	static UINT GetWndHeight(){ return GetInstance()->m_WndHeight; }

	// BackBufferの設定.
	static void SetBackBuffer();

	// ブレンドを有効:無効に設定する.
	static void SetBlend( bool EnableAlpha );
	// アルファカバレージを有効:無効に設定する.
	static void SetCoverage( bool EnableCoverage );
	// 深度テストを有効:無効に設定する.
	static void SetDeprh( bool flag );
	// ラスタライザステート設定.
	static void SetRasterizerState( const ERS_STATE& rsState );

	// フルスクリーンの設定.
	static bool SetFullScreen( const bool& isOn );
	// フルスクリーン状態か取得.
	static bool IsFullScreen();
	// ウィンドウサイズが変更された時に呼ぶ.
	static void Resize();

	// ビューポートの作成.
	HRESULT InitViewports( const D3D11_VIEWPORT& vp );

private: //----- 関数 ------.
		 // デバイス11の作成.
	HRESULT InitDevice11();
	// レンダーターゲットビューの作成.
	HRESULT InitTexRTV();
	// ステンシルビューの作成.
	HRESULT InitDSTex();
	// ビューポートの作成.
	HRESULT InitViewports();
	// ラスタライザの作成.
	HRESULT InitRasterizer();

	// ブレンド作成.
	HRESULT InitBlend();
	// 深度テスト作成.
	HRESULT InitDeprh();
	// ラスタライザステート作成.
	HRESULT InitRasterizerState();

private: //----- 変数 ------.
	HWND	m_hWnd;	// ウィンドウハンドル.

	ID3D11Device*			m_pDevice11;	// デバイス11.
	ID3D11DeviceContext*	m_pContext11;	// コンテキスト11.

	IDXGISwapChain*			m_pSwapChain;			// スワップチェーン.

	ID3D11RenderTargetView*	m_pBackBuffer_TexRTV;	// レンダーターゲットビュー.
	ID3D11DepthStencilView*	m_pBackBuffer_DSTexDSV;	// ステンシルビュー.
	ID3D11Texture2D*		m_pBackBuffer_DSTex;	// テクスチャー2D.


	ID3D11DepthStencilState*	m_pDepthStencilState;		// デプスステンシル有効.
	ID3D11DepthStencilState*	m_pDepthStencilStateOff;	// デプスステンシル無効.

	ID3D11BlendState*			m_pAlphaBlend;				// アルファブレンド有効.
	ID3D11BlendState*			m_pNoAlphaBlend;			// アルファブレンド無効.
	ID3D11BlendState*			m_pAlphaToCoverage;			// アルファカバレージ有効.

	ID3D11RasterizerState*		m_pRsSoldAndNone;	// ソリッドAnd正背面描画.
	ID3D11RasterizerState*		m_pRsSoldAndBack;	// ソリッドAnd背面を描画しない.
	ID3D11RasterizerState*		m_pRsSoldAndFront;	// ソリッドand正面を描画しない.
	ID3D11RasterizerState*		m_pRsWireFrame;		// ワイヤーフレーム描画.

	UINT	m_WndWidth;		// ウィンドウ幅.
	UINT	m_WndHeight;	// ウィンドウ高さ.

private:
	// コピー・ムーブコンストラクタ, 代入演算子の削除.
	CDirectX11( const CDirectX11 & )				= delete;
	CDirectX11& operator = ( const CDirectX11 & )	= delete;
	CDirectX11( CDirectX11 && )						= delete;
	CDirectX11& operator = ( CDirectX11 && )		= delete;
};

#endif	// #ifndef D3DX11_H.