/**
* @file Sprite.h.
* @brief スプライトクラス.
* @author 福田玲也.
*/
#ifndef SPRITE_H
#define SPRITE_H

#include "..\Common.h"
#include "SpriteStruct.h"

/****************************************
*	スプライトクラス.
**/
class CSprite : public CCommon
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
		ALIGN16 float		fFadeValue;		// フェード、トランジションの値.
	};
public:
	CSprite();
	virtual ~CSprite();

	// 初期化.
	virtual HRESULT Init( ID3D11DeviceContext* pContext11, const char* texturePath = nullptr );

	// 解放.
	void Release();

	// 描画.
	virtual void Render( SSpriteRenderState* pRenderState = nullptr );
	// 3D描画.
	virtual void Render3D( const bool& isBillboard = false, SSpriteRenderState* pRenderState = nullptr );

	// テクスチャの取得.
	inline ID3D11ShaderResourceView*	GetTexture()		const { return m_pSrcTexture; }
	// マスクテクスチャの設定.
	inline void SetDestTexture( ID3D11ShaderResourceView* pTexture ){ m_pDestTexture = pTexture; }
	// マスクテクスチャの設定.
	inline void SetMaskTexture( ID3D11ShaderResourceView* pTexture ){ m_pMaskTexture = pTexture; }

	// 頂点バッファの取得.
	inline ID3D11Buffer*				GetVertexBufferUI()	const { return m_pVertexBufferUI; }
	// 頂点バッファの取得.
	inline ID3D11Buffer*				GetVertexBuffer3D()	const { return m_pVertexBuffer3D; }
	// 描画座標の取得.
	inline D3DXVECTOR3					GetRenderPos()		const { return m_SState.vPos; }
	// 画像サイズの取得.
	inline WHDIZE_FLOAT					GetTextureSize()	const { return m_SState.Disp; }
	// テクスチャ座標の取得.
	inline SSpriteAnimState				GetAnimState()		const { return m_SpriteRenderState.AnimState; }
	// 画像の描画情報を取得.
	inline SSpriteRenderState			GetRenderState()	const { return m_SpriteRenderState; }

	// アニメーションの更新をするか.
	inline void SetAnimUpdate( const bool& isUpdate ){ m_SpriteRenderState.AnimState.IsAnimation = isUpdate; }
	// アニメーションの加算フレームを設定.
	inline void SetAnimFrame( const int& frame ){ m_SpriteRenderState.AnimState.FrameCountSpeed = frame; }

	// X方向のUVスクロールの更新をするか.
	inline void SetScrollUpdateX( const bool& isUpdate ){ m_SpriteRenderState.AnimState.IsUVScrollX = isUpdate; }
	// Y方向のUVスクロールの更新をするか.
	inline void SetScrollUpdateY( const bool& isUpdate ){ m_SpriteRenderState.AnimState.IsUVScrollY = isUpdate; }

	// X方向のUVスクロールの速度を設定.
	inline void SetScrollSpeedX( const float& speed ){ m_SpriteRenderState.AnimState.ScrollSpeed.x = speed; }
	// Y方向のUVスクロールの速度を設定.
	inline void SetScrollSpeedY( const float& speed ){ m_SpriteRenderState.AnimState.ScrollSpeed.y = speed; }

protected:
	// 頂点シェーダーの作成.
	HRESULT VertexInitShader( const char* shaderPath = nullptr );
	// ピクセルシェーダーの作成.
	virtual HRESULT PixelInitShader();
	// 定数バッファの作成.
	virtual HRESULT InitConstantBuffer();
	// サンプラの作成.
	HRESULT InitSample();

	// スプライト情報の読み込み.
	HRESULT SpriteStateRead( const char* filePath );
	// テクスチャ作成.
	HRESULT CreateTexture( const char* texturePath );
	// モデル作成 3D 用.
	HRESULT InitModel3D();
	// モデル作成 UI 用.
	HRESULT InitModelUI();

	// 頂点情報の作成.
	void CreateVertex(
		const float& w, const float& h,
		const float& u, const float& v, const bool& is3D );

	// テクスチャの比率を取得.
	int myGcd( int t, int t2 ) { if (t2 == 0) return t; return myGcd(t2, t % t2); }

protected:
	SSpriteRenderState				m_SpriteRenderState;
	ID3D11VertexShader*				m_pVertexShaderUI;						// 頂点シェーダー.
	ID3D11VertexShader*				m_pVertexShader3D;						// 頂点シェーダー.
	std::vector<ID3D11PixelShader*>	m_pPixelShaders;						// ピクセルシェーダー.
	ID3D11InputLayout*				m_pVertexLayout;						// 頂点レイアウト.
	ID3D11Buffer*					m_pConstantBuffer;						// コンスタントバッファ.
	ID3D11SamplerState*				m_pSampleLinears[ESamplerState_Max];	// サンプラ:テクスチャに各種フィルタをかける.

	ID3D11ShaderResourceView*	m_pSrcTexture;	// 通常テクスチャ.
	ID3D11ShaderResourceView*	m_pDestTexture;	// テクスチャ.
	ID3D11ShaderResourceView*	m_pMaskTexture;	// テクスチャ.

	ID3D11Buffer*		m_pVertexBuffer3D;	// 頂点バッファ.
	ID3D11Buffer*		m_pVertexBufferUI;	// 頂点バッファ.

	SSpriteState		m_SState;			// スプライト情報.

	SpriteVertex	m_Vertices[4];			// 頂点作成用.
};

#endif	// #ifndef SPRITE_H.
