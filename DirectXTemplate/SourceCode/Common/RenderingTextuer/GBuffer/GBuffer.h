#ifndef G_BUFFER_RENDER_H
#define G_BUFFER_RENDER_H

#include "..\RenderingTextuer.h"

/*********************************************
*	G-Bufferの描画クラス.
**/
class CGBufferRender : public CRenderTexture
{
public:
	// G-Bufferの種類.
	enum enGBufferNo
	{
		EGBufferNo_BACK,		// 色情報.
		EGBufferNo_COLOR,		// 色情報.
		EGBufferNo_NORMAL,		// 法線情報.
		EGBufferNo_WORLDPOS,	// ワールド座標.
		EGBufferNo_Z_DEPTH,		// 深度値情報.
		EGBufferNo_DIFFUSE,		// 色情報.

		EGBufferNo_MAX,	// 最大値.

	} typedef EGBufferNo;
public:
	CGBufferRender();
	virtual ~CGBufferRender();

	// 初期化.
	virtual HRESULT Init( ID3D11DeviceContext* pContext11 ) override;

	// バッファの設定.
	virtual void SetBuffer() override;
	// シェーダーリソースビューの数を取得.
	virtual int GetSRVCount() override { return EGBufferNo_MAX; }

protected:
	// テクスチャの初期化.
	virtual HRESULT InitBufferTex() override;
};

#endif	// #ifndef G_BUFFER_RENDER_H.