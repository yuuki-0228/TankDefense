#ifndef OUTLINE_RENDER_H
#define OUTLINE_RENDER_H

#include "..\RenderingTextuer.h"

/***********************************************
*	アウトライン描画クラス.
**/
class COutLineRender : public CRenderTexture
{
	// G-Bufferの種類.
	enum enOutLineNo
	{
		EOutLineNo_Default,		// アウトライン+色情報..
		EOutLineNo_LineOnly,	// アウトラインのみ.

		EOutLineNo_MAX,	// 最大値.

	} typedef EOutLineNo;

public:
	// コンスタントバッファ.
	struct OUTLINE_CBUFFER
	{
		// サンプル.
		struct OUTLINE_SAMPLE
		{
			float NormalPower;		// 法線のサンプルの強さ.
			float NormalCompare;	// 法線の比較値.
			float DepthPower;		// 深度値のサンプルの強さ.
			float DepthCompare;		// 深度値の比較値.
			OUTLINE_SAMPLE()
				: NormalPower	( 0.0f )
				, NormalCompare	( 0.0f )
				, DepthPower	( 0.0f )
				, DepthCompare	( 0.0f )
			{}
		};
		D3DXVECTOR4		Color;
		OUTLINE_SAMPLE	Sample;

		OUTLINE_CBUFFER()
			: Color		( 0.0f, 0.0f, 0.0f, 0.0f )
			, Sample	()
		{};
	};

public:
	COutLineRender();
	virtual ~COutLineRender();

	// 初期化.
	virtual HRESULT Init( ID3D11DeviceContext* pContext11 ) override;

	// 描画.
	void Render( const int& srvCount, const std::vector<ID3D11ShaderResourceView*>& srvList );

	// バッファの設定.
	virtual void SetBuffer() override;
	// シェーダーリソースビューの数を取得.
	virtual int GetSRVCount() override { return EOutLineNo_MAX; }

protected:
	// テクスチャの初期化.
	virtual HRESULT InitBufferTex() override;

private:
	ID3D11Buffer*	m_pConstantBufferFrame;	// コンスタントバッファ.
	OUTLINE_CBUFFER	m_OutlineBuffer;
};

#endif	// #ifndef OUTLINE_RENDER_H.