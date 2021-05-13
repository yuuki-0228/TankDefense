#ifndef CAMERA_MANAGER_H
#define CAMERA_MANAGER_H

#include "..\CameraBase.h"

class CFreeCamera;	// フリーカメラ.

/**************************************
*	カメラ管理クラス.
**/
class CCameraManager
{
public:
	CCameraManager();
	~CCameraManager();

	// インスタンスの取得.
	static CCameraManager* GetInstance();

	// 更新関数.
	static void Update( const float& deltaTime );

	// カメラを変更.
	static void ChangeCamera( CCameraBase* pCamera, const bool& isMove = 0, const float& sec = 1.0f );

	// 座標の取得.
	static D3DXVECTOR3 GetPosition(){ return GetInstance()->m_pCamera->GetPosition(); }
	// 視点座標の取得.
	static D3DXVECTOR3 GetLookPosition(){ return GetInstance()->m_pCamera->GetLookPosition(); }
	// ビュー行列取得関数.
	static D3DXMATRIX GetViewMatrix(){ return GetInstance()->m_pCamera->GetViewMatrix(); }
	// プロジェクション行列取得関数.
	static D3DXMATRIX GetProjMatrix(){ return GetInstance()->m_pCamera->GetProjMatrix(); }
	// ビュー・プロジェクションの合成行列取得関数.
	static D3DXMATRIX GetViewProjMatrix(){ return GetInstance()->m_pCamera->GetViewProjMatrix(); }

	// フリーカメラを有効化.
	static void SetActiveFreeCamera();
	// フリーカメラを無効化.
	static void SetPassiveFreeCamera();

	// カメラをnull初期化する.
	static void SetNullCamera();

private:
	// 切り替え移動の更新.
	void ChangeMoveUpdate( const float& deltaTime );

private:
	CCameraBase*					m_pCamera;
	CCameraBase*					m_pTmpCamera;
	std::unique_ptr<CFreeCamera>	m_pFreeCamera;
	CCameraBase::SCameraState		m_NewState;
	CCameraBase::SCameraState		m_OldState;
	float							m_MoveSeconds;
	float							m_MoveSecondsSpeed;
	bool							m_IsMoveCamera;
	bool							m_IsActiveFreeCamera;

private:
	// コピー・ムーブコンストラクタ, 代入演算子の削除.
	CCameraManager( const CCameraManager & )				= delete;
	CCameraManager& operator = ( const CCameraManager & )	= delete;
	CCameraManager( CCameraManager && )						= delete;
	CCameraManager& operator = ( CCameraManager && )		= delete;
};

#endif	// #ifndef CAMERA_MANAGER_H.