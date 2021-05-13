#ifndef LIGHT_MANAGER_H
#define LIGHT_MANAGER_H

#include "..\LightBase.h"
#include <queue>

class CNormalLight;

class CLightManager
{
public:
	CLightManager();
	~CLightManager();

	// インスタンスの取得.
	static CLightManager* GetInstance();

	// 更新関数.
	static void Update( const float& deltaTime );

	// 座標の取得.
	static D3DXVECTOR3 GetPosition(){ return GetInstance()->m_pLight->GetPosition(); }
	// 視点座標の取得.
	static D3DXVECTOR3 GetLookPosition(){ return GetInstance()->m_pLight->GetLookPosition(); }
	// 方向を取得.
	static D3DXVECTOR3 GetDirection(){ return GetInstance()->m_pLight->GetDirection(); }
	// 色を取得.
	static D3DXVECTOR3 GetColor(){ return GetInstance()->m_pLight->GetColor(); }
	// 強さを取得.
	static float GetIntensity(){ return GetInstance()->m_pLight->GetIntensity(); }

	// ライトキューの取得.
	static std::queue<SLightState> GetLightStateQueue(){ return GetInstance()->m_LightStateQueue; }
	// ライトキューに追加.
	static void PushLightStateQueue( const SLightState& lightState ){ return GetInstance()->m_LightStateQueue.push(lightState); }
	static void LightStateQueueClear()
	{
		// 余ったキューの後片付け.
		const int queueSize = static_cast<int>(GetInstance()->m_LightStateQueue.size());
		for( int i = 0; i < queueSize; i++ ){
			GetInstance()->m_LightStateQueue.pop();
		}
	}
	// 位置の描画.
	static void PositionRender();

private:
	CLightBase*						m_pLight;
	std::unique_ptr<CNormalLight>	m_pNormalLight;
	std::queue<SLightState>			m_LightStateQueue;
#ifdef _DEBUG
	CDX9StaticMesh* m_StaticMesh;
#endif // #ifdef _DEBUG.

private:
	// コピー・ムーブコンストラクタ, 代入演算子の削除.
	CLightManager( const CLightManager & )				= delete;
	CLightManager& operator = ( const CLightManager & )	= delete;
	CLightManager( CLightManager && )					= delete;
	CLightManager& operator = ( CLightManager && )		= delete;
};

#endif	// #ifndef LIGHT_MANAGER_H.