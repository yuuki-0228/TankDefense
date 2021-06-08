/**
* @file SpriteRender.h.
* @brief スプライト描画クラス.
* @author 福田玲也.
*/
#ifndef SPRITE_RENDER_H
#define SPRITE_RENDER_H

#include "..\Common.h"
#include "SpriteStruct.h"
#include <queue>

class CSprite;

using spriteRenderPair = std::pair<CSprite*, SSpriteRenderState>;

/***************************************
*	画像描画クラス.
*		画像情報はCSpriteクラスが持っている.
**/
class CSpriteRender : public CCommon
{
	// コンスタントバッファのアプリ側の定義.
	//※シェーダー内のコンスタントバッファと一致している必要あり.
	struct C_BUFFER
	{
		ALIGN16 D3DXMATRIX	mW;				// ワールド行列.
		ALIGN16 D3DXMATRIX	mWVP;			// ワールド・ビュー・プロジェクション.
		ALIGN16 D3DXVECTOR4 vColor;			// カラー(RGBAの型に合わせる).
		ALIGN16 D3DXVECTOR2 vUV;			// UV座標.
		ALIGN16 D3DXVECTOR2 vViewPort;		// ビューポート.
	};
public:
	CSpriteRender();
	virtual ~CSpriteRender();

	// 初期化.
	HRESULT Init( ID3D11DeviceContext* pContext11 );

	// 解放.
	void Release();

	// 描画.
	void Render();
	void Render(
		const SSpriteRenderState& renderState, 
		ID3D11ShaderResourceView* pTexture, 
		ID3D11Buffer* pVertexBufferUI );

	// 画像データの追加.
	void PushSpriteData( const spriteRenderPair& renderPair );
	void PushSpriteData( CSprite* pSprite, const SSpriteRenderState& renderState );
	void PushSpriteData( CSprite* pSprite );

private:
	// シェーダーの作成.
	HRESULT InitShader();
	// 定数バッファの作成.
	HRESULT InitConstantBuffer();
	// サンプラの作成.
	HRESULT InitSample();

private:
	std::queue<spriteRenderPair>	m_pSpriteQueue;							// 画像キュー.
	ID3D11VertexShader*				m_pVertexShader;						// 頂点シェーダー.
	ID3D11PixelShader*				m_pPixelShader;							// ピクセルシェーダー.
	ID3D11InputLayout*				m_pVertexLayout;						// 頂点レイアウト.
	ID3D11Buffer*					m_pConstantBuffer;						// コンスタントバッファ.
	ID3D11SamplerState*				m_pSampleLinears[ESamplerState_Max];	// サンプラ:テクスチャに各種フィルタをかける.
};

#endif	// #ifndef SPRITE_RENDER_H.