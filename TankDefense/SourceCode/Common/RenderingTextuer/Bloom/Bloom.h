#ifndef BLOOM_RENDER_H
#define BLOOM_RENDER_H

#include "..\RenderingTextuer.h"

class CDownSamplingRender;

/***********************************************
*	ブルーム描画クラス.
**/
class CBloomRender : public CRenderTexture
{
	struct C_BUFFER_PER_FRAME
	{
		D3DXVECTOR4 SoftKneePram;
	};
public:
	CBloomRender();
	virtual ~CBloomRender();

	// 初期化.
	virtual HRESULT Init( ID3D11DeviceContext* pContext11 ) override;

	// 描画.
	void Render( const int& srvCount, const std::vector<ID3D11ShaderResourceView*>& srvList );

	// バッファの設定.
	virtual void SetBuffer() override;
	// シェーダーリソースビューの数を取得.
	virtual int GetSRVCount() override { return m_pShaderResourceViewList.size(); }

protected:
	// テクスチャの初期化.
	virtual HRESULT InitBufferTex() override;

private:
	// 頂点バッファーの作成.
	virtual HRESULT InitVertexBuffer() override;
	// コンスタントバッファの作成.
	virtual HRESULT InitConstantBuffer() override;

private:
	std::unique_ptr<CDownSamplingRender>	m_pDownSampling;		// ダウンサンプル.
	std::vector<ID3D11Buffer*>				m_pConstantBuffersInit;	// コンスタントバッファ.
	std::vector<ID3D11Buffer*>				m_pVertexBuffers;		// 頂点バッファ.
};

#endif	// #ifndef BLOOM_RENDER_H.