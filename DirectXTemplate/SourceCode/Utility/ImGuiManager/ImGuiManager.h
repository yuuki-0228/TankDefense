/**
* @file ImGuiManager.h.
* @brief ImGuiを管理するクラス.
* @author 福田玲也.
*/
#ifndef IMGUI_MANAGER_H
#define IMGUI_MANAGER_H
// 警告についてのコード分析を無効にする.4005:再定義.
#pragma warning(disable:4005)

#include <Windows.h>
#include <D3DX11.h>
#include <D3D11.h>

#include <memory>
#include <queue>
#include <functional>

// ライブラリ読み込み.
#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "d3dx11.lib" )
#pragma comment( lib, "d3d11.lib" )

#include "..\..\..\Library\ImGui\imgui.h"
#include "..\..\..\Library\ImGui\imgui_impl_dx11.h"
#include "..\..\..\Library\ImGui\imgui_impl_win32.h"
#include "..\..\..\Library\ImGui\imgui_internal.h"
#include "..\..\Utility\FrameRate\FrameRate.h"


class CImGuiManager
{
public:
	struct stSuccess
	{
		float	FrameTime;		// カウント.
		float	RenderTime;		// 描画時間.
		bool	IsSucceeded;	// 成功かどうか.
		stSuccess()
			: FrameTime		( 0.0f )
			, RenderTime	( 5.0f )
			, IsSucceeded	( false )
		{}
		void Render()
		{
			if( IsSucceeded == false ) return;
			ImGui::Text( u8"成功" );
			ImGui::SameLine();
			FrameTime += static_cast<float>(CFrameRate::GetDeltaTime());
			if( FrameTime > RenderTime ) return;
			FrameTime = 0.0f;
			IsSucceeded = false;
		}
	} typedef SSuccess;

public:
	CImGuiManager();
	~CImGuiManager();

	// インスタンスの取得.
	static CImGuiManager* GetInstance();

	// 初期化関数.
	static HRESULT Init( 
		HWND hWnd, 
		ID3D11Device* pDevice11,
		ID3D11DeviceContext* pContext11 );

	// 解放関数.
	static HRESULT Release();

	// フレームの設定.
	static void SetingNewFrame();

	// 描画.
	static void Render();

	// 描画関数を追加する.
	static void PushRenderProc( std::function<void()> proc );

	// ドラッグの表示.
	static void DragDouble( const char* label, double* v, const double& speed = 0.001 );
	static void DragFloat( const char* label, float* v, const float& speed = 0.001f );
	static void DragInt( const char* label, int* v, const float& speed = 1.0f );

private:
	std::queue<std::function<void()>> m_RenderQueue;
	bool m_IsRender;
};

#endif	// #ifndef IMGUI_MANAGER_H.