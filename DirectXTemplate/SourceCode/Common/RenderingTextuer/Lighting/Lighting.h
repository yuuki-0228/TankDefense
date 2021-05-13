#ifndef LIGHTING_RENDER_H
#define LIGHTING_RENDER_H

#include "..\RenderingTextuer.h"

/***********************************************
*	ライティング描画クラス.
**/
class CLightingRender : public CRenderTexture
{
	// "Lighting_PS.hlsl"のライト最大値と値を合わせることが必要.
	static constexpr int LIGHT_MAX_COUNT = 2;	// ライトの最大数.

	// コンスタントバッファ.
	struct LIGHT_CBUFFER
	{
		D3DXVECTOR4 CameraPos;
		D3DXVECTOR4 LightPos[LIGHT_MAX_COUNT];
		D3DXVECTOR4 LightVec[LIGHT_MAX_COUNT];
		D3DXVECTOR4 LightColor[LIGHT_MAX_COUNT];
		D3DXVECTOR4 LightConeAngle[LIGHT_MAX_COUNT];
		D3DXVECTOR4 LightType[LIGHT_MAX_COUNT];
		LIGHT_CBUFFER()
			: CameraPos			( 0.0f, 0.0f, 0.0f, 0.0f )
			, LightPos			()
			, LightVec			()
			, LightColor		()
			, LightConeAngle	()
			, LightType			()
		{
			for( int i = 0; i < LIGHT_MAX_COUNT; i++ ){
				LightPos[i]			= { 0.0f, 0.0f, 0.0f, 0.0f };
				LightVec[i]			= { 0.0f, 0.0f, 0.0f, 0.0f };
				LightColor[i]		= { 0.0f, 0.0f, 0.0f, 0.0f };
				LightConeAngle[i]	= { 0.0f, 0.0f, 0.0f, 0.0f };
				LightType[i]		= { 0.0f, 0.0f, 0.0f, 0.0f };
			}
		};
	};

public:
	CLightingRender();
	virtual ~CLightingRender();

	// 初期化.
	virtual HRESULT Init( ID3D11DeviceContext* pContext11 ) override;

	// 描画.
	void Render( const int& srvCount, const std::vector<ID3D11ShaderResourceView*>& srvList );

	// バッファの設定.
	virtual void SetBuffer() override;
	// シェーダーリソースビューの数を取得.
	virtual int GetSRVCount() override { return 1; }

protected:
	// テクスチャの初期化.
	virtual HRESULT InitBufferTex() override;

private:
	ID3D11Buffer*	m_pConstantBufferFrame;	// コンスタントバッファ.
};

#endif	// #ifndef LIGHTING_RENDER_H.