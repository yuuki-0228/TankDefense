/**
* @file D3DX9.h.
* @brief DirectX9デバイスクラス.
* @author 福田玲也.
*/
#ifndef D3DX9_H
#define D3DX9_H

#include "..\..\Global.h"

/*********************************
*	DirectX9.
**/
class CDirectX9
{
public:
	CDirectX9();
	~CDirectX9();

	// インスタンスの取得.
	static CDirectX9* GetInstance();

	// DirectX9の構築.
	static HRESULT Create( HWND hWnd );
	// DirectX9の解放.
	static HRESULT Release();

	// デバイスの取得関数.
	static LPDIRECT3DDEVICE9 GetDevice(){ return GetInstance()->m_pDevice9; }

private:
	// デバイス9の構築.
	HRESULT CreateDevice9();

private:
	HWND				m_hWnd;		// ウィンドウハンドル.
	LPDIRECT3DDEVICE9	m_pDevice9;	// デバイス9.

private:
	// コピー・ムーブコンストラクタ, 代入演算子の削除.
	CDirectX9( const CDirectX9 & )				= delete;
	CDirectX9& operator = ( const CDirectX9 & )	= delete;
	CDirectX9( CDirectX9 && )					= delete;
	CDirectX9& operator = ( CDirectX9 && )		= delete;
};

#endif	// #ifndef D3DX9_H.