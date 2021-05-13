#pragma once

//警告についてのｺｰﾄﾞ分析を無効にする.4005:再定義.
#pragma warning(disable:4005)
#include <Windows.h>
#include <crtdbg.h>
#include <memory>

#include <D3DX11.h>
#include <D3D11.h>
#include <D3DX10.h>	//「D3DX～」の定義使用時に必要.
#include <D3D10.h>
#include <d3dx9.h>

#include "Utility/Log/Log.h"
#include "Utility/Tranceform/Tranceform.h"
#include "Utility/FrameRate/FrameRate.h"

// ライブラリ読み込み.
#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "d3dx11.lib" )
#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "d3dx10.lib" )	//「D3DX～」の定義使用時に必要.
#pragma comment( lib, "d3d9.lib" )
#pragma comment( lib, "d3dx9.lib" )

//=================================================
//	定数.
//=================================================
constexpr int WND_W	= 1280;	// ウィンドウの幅.
constexpr int WND_H	= 720;	// ウィンドウの高さ.
constexpr int FPS	= 60;

// 初期化用 0値.
constexpr int		FOR_INIT_ZERO_INT		= 0;
constexpr float		FOR_INIT_ZERO_FLOAT		= 0.0f;
constexpr double	FOR_INIT_ZERO_DOUBLE	= 0.0;
constexpr float		FOR_INIT_ZERO_VEC3[3]	= { 0.0f, 0.0f, 0.0f };

class CCamera;
class CDX9SkinMesh;
class CDX9StaticMesh;
class CSprite;
class CLightBase;
class CSpriteRender;

//===========================================================
//	マクロ.
//===========================================================
// 解放.
#define SAFE_RELEASE(p)			if(p!=nullptr){(p)->Release();(p)=nullptr;}
// 破棄.
#define SAFE_DELETE(p)			if(p!=nullptr){delete (p);(p)=nullptr;}
#define SAFE_DELETE_ARRAY(p)	if(p!=nullptr){delete[] (p);(p) = nullptr;}
// _declspec()	:DLLから(関数,クラス,クラスのメンバ関数を)エクスポートする.
// align()		:(強制的に)16byte で使用する.
#define ALIGN16	_declspec(align(16))

#include <string>
#include <codecvt>

template<class T = std::string >
void ERROR_MESSAGE ( const std::string& error )
{
	if( error.empty() ){
		_ASSERT_EXPR( false, L"Not Error Message" );
	}

	// w_charへの変換.
	int size_needed = MultiByteToWideChar( CP_ACP, 0, &error[0], (int)error.size(), NULL, 0 );
	std::wstring wstrTo( size_needed, 0 );
	MultiByteToWideChar( CP_ACP, 0, &error[0], (int)error.size(), &wstrTo[0], size_needed );

	CLog::Print( error.c_str() );
	_ASSERT_EXPR( false, wstrTo.c_str() );
	MessageBox( nullptr, error.c_str(), "Warning", MB_OK );
}