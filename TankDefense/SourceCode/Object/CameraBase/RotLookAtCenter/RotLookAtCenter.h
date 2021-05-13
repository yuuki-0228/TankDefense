/**
* @file RotLookAtCenter.h.
* @brief ターゲットを中心に回転するカメラクラス.
* @author 福田玲也.
*/
#ifndef ROTATION_LOOK_AT_CENTER_CAMERA_H
#define ROTATION_LOOK_AT_CENTER_CAMERA_H

#include "..\CameraBase.h"

/********************************************
*	ターゲットを中心に回転するカメラクラス.
**/
class CRotLookAtCenter : public CCameraBase
{
private: //------ 定数 -------.
	static constexpr float DEFAULT_LENGTH			= 20.0f;	// カメラ位置と注視位置の距離.
	static constexpr float DEFAULT_HORIZONTAL_MAX	= 360.0f;	// 横回転の最大サイズ.
	static constexpr float DEFAULT_HORIZONTAL_MIN	= 0.0f;		// 横回転の最小サイズ.
	static constexpr float DEFAULT_VERTICAL_MAX		= 90.0f;	// 縦回転の最大サイズ.
	static constexpr float DEFAULT_VERTICAL_MIN		= 0.0f;		// 縦回転の最小サイズ.
	static constexpr float DEFAULT_MOVE_SPEED		= 0.01f;	// 移動速度.

public:
	// カメラ設定ファイルのパス.
	inline static const char*	CONFIG_FILE_PATH		= "Data\\Config\\Camera.bin";
	// カメラ設定の状態.
	struct stConfigState
	{
		bool	IsReverse;	// 反転するか.
		float	MoveSpeed;	// 移動速度.

		stConfigState()
			: IsReverse	( false )
			, MoveSpeed	( 0.01f )
		{}
	} typedef SConfigState;

public:  //------ 関数 -------.
	CRotLookAtCenter();
	CRotLookAtCenter( const bool& isLoad );
	virtual ~CRotLookAtCenter();

	// パラメーターの読み込み.
	bool LoadParameter( const bool& isLoad );

	// 更新.
	virtual void Update( const float& deltaTime ) override;

	// オブジェクトをを注視に回転.
	void RotationLookAtObject( const D3DXVECTOR3& vLookPos, const float& attenRate );

	// 横方向の回転.
	void DegreeHorizontalMove( const float& movePower );
	// 横方向の右回転.
	void DegreeHorizontalRightMove();
	// 横方向の左回転.
	void DegreeHorizontalLeftMove();
	// 縦方向の回転.
	void DegreeVerticalMove( const float& movePower );
	// ラジアンの取得関数.
	float GetRadianX() const { return m_vDegree.x; }
	// カメラと注視点の長さ設定.
	void SetLength( const float& length );
	// 高さの設定.
	void SetHeight( const float& height );
	// 横方向の最大、最小の設定.
	void SetHorizontalDegree( const float& max, const float& min );
	// 縦方向の最大、最小の設定.
	void SetVerticalDegree( const float& max, const float& min );
	// 座標に足し合わせる値の設定.
	void SetAddPositionValue( const D3DXVECTOR3& addValue );

private: //------ 変数 -------.
	D3DXVECTOR2		m_vDegree;			// カメラの角度.
	float			m_Length;			// カメラと注視点の長さ.
	float			m_HorizontalMax;	// 横方向の最大サイズ.
	float			m_HorizontalMin;	// 横方向の最小サイズ.
	float			m_VerticalMax;		// 縦方向の最大サイズ.
	float			m_VerticalMin;		// 縦方向の最小サイズ.
	float			m_RightMoveSpeed;	// 右の移動速度.
	float			m_LeftMoveSpeed;	// 左の移動速度.
	D3DXVECTOR3		m_AddValue;
};

#endif	// #ifndef ROTATION_LOOK_AT_CENTER_CAMERA_H.