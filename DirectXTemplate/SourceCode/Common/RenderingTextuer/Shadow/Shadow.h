#ifndef SHADOW_H
#define SHADOW_H

#include "..\RenderingTextuer.h"

class CShadow : public CRenderTexture
{
public:
	CShadow();
	virtual ~CShadow();

	// 初期化.
	virtual HRESULT Init( ID3D11DeviceContext* pContext11 ) override;

	// バッファの設定.
	virtual void SetBuffer() override;
	// シェーダーリソースビューの数を取得.
	virtual int GetSRVCount() override;


	// テクスチャの初期化.
	virtual HRESULT InitBufferTex() override;
};

#endif	// #ifndef SHADOW_H.