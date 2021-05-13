#ifndef WATERCOLOR_PAINTING_RENDER_H
#define WATERCOLOR_PAINTING_RENDER_H

#include "..\RenderingTextuer.h"

/***********************************************
*	ライティング描画クラス.
**/
class CWatercolorPaintingRender : public CRenderTexture
{
	// コンスタントバッファ.
	struct LIGHT_CBUFFER
	{
		D3DXVECTOR4 Color;
		LIGHT_CBUFFER()
		{
		};
	};

public:
	CWatercolorPaintingRender();
	virtual ~CWatercolorPaintingRender();

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

#endif	// #ifndef WATERCOLOR_PAINTING_RENDER_H.