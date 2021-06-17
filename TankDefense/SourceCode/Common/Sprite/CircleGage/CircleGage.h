#ifndef CIRCLE_GAGE_H
#define CIRCLE_GAGE_H

#include "..\Sprite.h"

/**************************************
*	ゲージスプライト用クラス.
**/
class CCircleGageSprite : public CSprite
{
	struct CBUFFER_PER_FRAME
	{
		ALIGN16 D3DXMATRIX	mW;				// ワールド行列.
		ALIGN16 D3DXMATRIX	mWVP;			// ワールド行列.
		ALIGN16 D3DXVECTOR4 vColor;			// 色.
		ALIGN16 D3DXVECTOR2	vUV;			// UV座標.
		ALIGN16 D3DXVECTOR2	vViewPort;		// ビューポート.
		ALIGN16 D3DXVECTOR2	vCenterPos;		// 円の中心座標 : 0~1.
		ALIGN16 D3DXVECTOR2	vStartVector;	// 円の開始ベクトル.
		ALIGN16 float		Value;			// 値.
		ALIGN16 float		IsClockwise;	// 時計周りか.
	};

public:
	// ゲージの種類.
	enum enGageNo : unsigned char
	{
		EGageNo_None,

		EGageNo_Circle = 0,	// サークルゲージ.

		EGageNo_Max,
	} typedef EGageNo;

public:
	CCircleGageSprite();
	virtual ~CCircleGageSprite();

	// 初期化.
	virtual HRESULT Init( ID3D11DeviceContext* pContext11 = nullptr, const char* texturePath = nullptr ) override;

	// 解放.
	void Release();

	// 描画.
	virtual void Render( SSpriteRenderState* pRenderState = nullptr ) override;
	// 3D描画.
	virtual void Render3D( const bool& isBillboard = false, SSpriteRenderState* pRenderState = nullptr ) override;

	// センター座標の設定.
	//	0 ~ 1の値.
	//	スプライトのローカル座標が中心で、
	//	スプライトの中心にサークルを置きたい場合、
	//	座標を(0.5, 0.5)に設定すれば良い.
	inline void SetCenterPosition( const D3DXVECTOR2& pos ){ m_CenterPosition = pos; }

	// サークルの開始ベクトルの設定.
	//	-1 ~ 1の値.
	//	上から開始したい場合は(0,-1)に設定する.
	inline void SetStartVector( const D3DXVECTOR2& vec ){ m_StartVector = vec; }

	// 値の設定 : 0 ~ 1 の値.
	inline void SetValue( const float& value ){ m_Value = value; }

	// サークルの角度の設定.
	//	0 ~ 360の値.
	inline void SetAngleDgree( const float& deg )	{ m_Angle = static_cast<float>(D3DXToRadian(deg)); }
	// サークルの角度の設定.
	//	0 ~ PI*2の値.
	inline void SetAngleRadian( const float& rad )	{ m_Angle = rad; }

	// 時計周りかどうか設定.
	inline void SetIsClockwise( const bool& isClockwise )	{ m_IsClockwise = isClockwise; }

	// テクスチャの設定.
	inline void SetSrcTexture( ID3D11ShaderResourceView* pTexture ){ m_pSrcTexture = pTexture; }
	// 頂点バッファの設定.
	inline void SetVertexBufferUI( ID3D11Buffer* pBuffer ){ m_pVertexBufferUI = pBuffer; }
	// 頂点バッファの設定.
	inline void SetVertexBuffer3D( ID3D11Buffer* pBuffer ){ m_pVertexBuffer3D = pBuffer; }

private:
	// ピクセルシェーダーの作成.
	virtual HRESULT PixelInitShader() override;
	// 定数バッファの作成.
	virtual HRESULT InitConstantBuffer()override;

private:
	D3DXVECTOR2		m_CenterPosition;	// 円の中心座標.
	D3DXVECTOR2		m_StartVector;		// 開始ベクトル.
	float			m_Value;			// ゲージの値.
	float			m_Angle;			// サークルの角度(ラジアン).
	bool			m_IsClockwise;		// 時計回りか.
};

#endif	// #ifndef CIRCLE_GAGE_H.