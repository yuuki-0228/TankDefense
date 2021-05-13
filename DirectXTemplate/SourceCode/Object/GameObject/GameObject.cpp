#include "GameObject.h"
#include "..\CameraBase\CameraManager\CameraManager.h"

CGameObject::CGameObject()
	: m_DeltaTime	( FOR_INIT_ZERO_FLOAT )
	, m_ObjectTag	( EObjectTag::None )
{
}

CGameObject::~CGameObject()
{
}

//--------------------------------.
// 画面の外に出ているか.
//--------------------------------.
bool CGameObject::IsDisplayOut( const float& adjSize )
{
	const float wnd_w = static_cast<float>(CDirectX11::GetWndWidth());
	const float wnd_h = static_cast<float>(CDirectX11::GetWndHeight());
	// ビューポート行列.
	D3DXMATRIX mViewport;
	D3DXMatrixIdentity( &mViewport );	// 単位行列作成.
	mViewport._11 = wnd_w *  0.5f;
	mViewport._22 = wnd_h * -0.5f;
	mViewport._41 = wnd_w *  0.5f;
	mViewport._42 = wnd_h *  0.5f;

	// ワールド, ビュー, プロジェ, ビューポートの計算.
	D3DXMATRIX m = CCameraManager::GetViewMatrix() * CCameraManager::GetProjMatrix() * mViewport;
	D3DXVECTOR3 screenPos = { 0.0f, 0.0f, 0.0f };	// スクリーン座標.
	// D3DXVECTOR3と掛け合わせ.
	D3DXVec3TransformCoord( &screenPos, &m_Tranceform.Position, &m );

	if( -adjSize < screenPos.x && screenPos.x < wnd_w + adjSize &&
		-adjSize < screenPos.y && screenPos.y < wnd_h + adjSize ) return false;	// 画面内.
	
	return true;	// 画面外.
}