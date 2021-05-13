#ifndef DOWN_SAMPLING_RENDER_H
#define DOWN_SAMPLING_RENDER_H

#include "..\RenderingTextuer.h"

/***********************************************
*	Bloom用に明度を落とすクラス.
**/
class CDownSamplingRender : public CRenderTexture
{
public:
	struct stDownSamplePrame
	{
		float SoftThrshold;
		float Thrshold;
		float Intensity;

		stDownSamplePrame()
			: SoftThrshold	( 1.0f )
			, Thrshold		( 0.55f )
			, Intensity		( 1.0f )
		{}
	} typedef SDownSamplePrame;

private:
	struct C_BUFFER_PER_FRAME
	{
		D3DXVECTOR4 SoftKneePram;
		D3DXVECTOR4 Intensity;
	};

public:
	CDownSamplingRender();
	virtual ~CDownSamplingRender();

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
	ID3D11Buffer*		m_pConstantBufferFrame;	// コンスタントバッファ.
	SDownSamplePrame	m_DownSamleParam;
};

#endif	// #ifndef DOWN_SAMPLING_RENDER_H.