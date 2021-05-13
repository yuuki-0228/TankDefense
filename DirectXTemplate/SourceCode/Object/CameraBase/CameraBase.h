/**
* @file CameraBase.h.
* @brief カメラ基底クラス.
* @author 福田玲也.
*/
#ifndef CAMERA_BASE_H
#define CAMERA_BASE_H

#include "..\Object.h"

/****************************************
*	カメラベースクラス.
**/
class CCameraBase : public CObject
{
public:
	// カメラ情報.
	struct stCameraState
	{
		D3DXVECTOR3 Position;		// 座標.
		D3DXVECTOR3 LookPosition;	// 視点座標.
	} typedef SCameraState;

public:
	CCameraBase();
	virtual ~CCameraBase();

	// 更新関数.
	virtual void Update( const float& deltaTime ) = 0;

	// 指定秒で、指定座標に移動するカメラ動作.
	void AnySecondsMove( const SCameraState& newState, const SCameraState& oldState, const float& sec );
	// 指定秒で、指定座標に移動するカメラ動作.
	void AnySecondsMovePosition( const D3DXVECTOR3& newPos, const D3DXVECTOR3& oldPos, const float& sec );
	// 指定秒で、指定座標に移動するカメラ動作.
	void AnySecondsMoveLookPosition( const D3DXVECTOR3& newPos, const D3DXVECTOR3& oldPos, const float& sec );

	// ビュー・プロジェクションの更新.
	void UpdateViewProj();

	// 注視座標の設定.
	inline void SetLookPosition( const D3DXVECTOR3& p ) { m_LookPosition = p; }
	// 注視座標の取得.
	inline D3DXVECTOR3 GetLookPosition() const { return m_LookPosition; }

	// ビュー行列取得関数.
	inline D3DXMATRIX GetViewMatrix() const { return m_ViewMatrix; }
	// プロジェクション行列取得関数.
	inline D3DXMATRIX GetProjMatrix() const { return m_ProjMatrix; }
	// ビュー・プロジェクションの合成行列取得関数.
	inline D3DXMATRIX GetViewProjMatrix() const { return m_ViewMatrix * m_ProjMatrix; }

	// 動作状態か.
	inline bool IsActive() const { return m_IsActive; }

protected:
	D3DXMATRIX	m_ViewMatrix;			// ビュー行列.
	D3DXMATRIX	m_ProjMatrix;			// プロジェクション行列.
	D3DXVECTOR3	m_LookPosition;			// カメラ注視座標.
	D3DXVECTOR3	m_ViewVector;			// ビューベクトル.
	float		m_ViewAngle;			// 視野角.
	float		m_Aspect;				// アスペクト比.
	float		m_MaxClippingDistance;	// 最大クリッピング距離.
	float		m_MinClippingDistance;	// 最小クリッピング距離.
	bool		m_IsActive;				// 動作しているか.
};

#endif	// #ifndef CAMERA_BASE_H.