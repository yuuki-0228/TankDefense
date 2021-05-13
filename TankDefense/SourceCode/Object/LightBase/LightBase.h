#ifndef LIGHT_BASE_H
#define LIGHT_BASE_H

#include "..\Object.h"

// ライト情報.
struct stLightState
{
	static constexpr int NONE_LIGHT			= 0;	// ライト無し.
	static constexpr int DIRECTIONAL_LIGHT	= 1;	// ディレクショナルライト.
	static constexpr int POINT_LIGHT		= 2;	// ポイントライト.
	static constexpr int SPOT_LIGHT			= 3;	// スポットライト.

	D3DXVECTOR4 Position;	// 座標.
	D3DXVECTOR4 Vector;		// 方向.
	D3DXVECTOR4 Color;		// 色 wを強さとして使用.
	D3DXVECTOR4 ConeAngle;	// スポットライトのコーン値.
	int Type;				// ライトの種類.

	stLightState()
		: Position	( 0.0f, 0.0f, 0.0f, 0.0f )
		, Vector	( 0.0f, 0.0f, 0.0f, 0.0f )
		, Color		( 0.0f, 0.0f, 0.0f, 0.0f )
		, ConeAngle	( 0.0f, 0.0f, 0.0f, 0.0f )
		, Type		( NONE_LIGHT )
	{}

} typedef SLightState;

/*************************************************
*	ライトベースクラス.
**/
class CLightBase : public CObject
{
public:
	CLightBase();
	virtual ~CLightBase();

	// 更新関数.
	virtual void Update( const float& deltaTime ) = 0;

	// 注視座標の設定.
	inline void SetLookPosition( const D3DXVECTOR3& p ) { m_LookPosition = p; }
	// 注視座標の取得.
	inline D3DXVECTOR3	GetLookPosition()	const { return m_LookPosition; }
	// 方向を取得.
	inline D3DXVECTOR3	GetDirection()		const { return m_Direction; }
	// ライトの色を取得.
	inline D3DXVECTOR3	GetColor()			const { return m_Color; }
	// ライトの強さを取得.
	inline float		GetIntensity()		const { return m_Intensity; }

	// 動作しているか
	inline bool			IsActive()			const { return m_IsActive; }

protected:
	// シンプルな移動.
	void SimpleMove( const float& deltaTime );

protected:
	D3DXVECTOR3	m_LookPosition;	// 視点座標.
	D3DXVECTOR3	m_Direction;	// ライト方向.
	D3DXVECTOR3	m_Color;		// ライトの色.
	float		m_Intensity;	// ライトの強さ.
	bool		m_IsActive;		// 動作しているか.
};

#endif	// #ifndef LIGHT_BASE_H.