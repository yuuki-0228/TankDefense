#ifndef RENDER_TEXTURE_H
#define RENDER_TEXTURE_H

#include "..\Common.h"

/************************************************
*	レンダリングテクスチャのベースクラス.
**/
class CRenderTexture : public CCommon
{
protected:
	// 頂点の構造体.
	struct VERTEX
	{
		D3DXVECTOR3 Pos;	// 頂点座標.
		D3DXVECTOR2 Tex;	// テクスチャ座標.
	};
	// コンスタントバッファ.
	struct C_BUFFER_PER_INIT
	{
		ALIGN16 D3DXMATRIX	mW;				// ワールド行列.
		ALIGN16 D3DXVECTOR2	vViewPort;		// ウィンドウサイズ.
		ALIGN16 D3DXVECTOR2	vPixelSize;		// ピクセルのサイズ.
		ALIGN16 D3DXVECTOR2	vWndRatio;		// ウィンドウの横幅の比率.
	};

	const float CLEAR_BACK_COLOR[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
public:
	CRenderTexture();
	virtual ~CRenderTexture();

	// 初期化.
	virtual HRESULT Init( ID3D11DeviceContext* pContext11 ) = 0;

	// 解放.
	void Release();

	// バッファの設定.
	virtual void SetBuffer() = 0;

	// シェーダーリソースビューの取得.
	inline std::vector<ID3D11ShaderResourceView*> GetShaderResourceViewList() const { return m_pShaderResourceViewList; }
	// シェーダーリソースビューの数を取得.
	virtual int GetSRVCount() = 0;

protected:
	// 初期化.
	HRESULT InitBase( ID3D11DeviceContext* pContext11 );
	// バッファの設定.
	void SetBuffer( const int& numViews );

	// テクスチャの初期化.
	virtual HRESULT InitBufferTex() = 0;

	// 頂点バッファーの作成.
	virtual HRESULT InitVertexBuffer();
	// コンスタントバッファの作成.
	virtual HRESULT InitConstantBuffer();
	// 頂点シェーダー作成.
	HRESULT InitVertexShader( const char* filePath, const char* entryName );
	// ピクセルシェーダー作成.
	HRESULT InitPixelShader( const char* filePath, const char* entryName );

	// サンプラーの作成.
	HRESULT InitSampleLinear();

	// バッファの作成.
	HRESULT CreateBufferTex(
		const D3D11_TEXTURE2D_DESC	texDesc,
		ID3D11RenderTargetView**	ppRTV,
		ID3D11ShaderResourceView**	ppSRV,
		ID3D11Texture2D**			ppTex );

protected:
	std::vector<ID3D11RenderTargetView*>	m_pRenderTargetViewList;	// レンダーターゲットビュー.
	std::vector<ID3D11ShaderResourceView*>	m_pShaderResourceViewList;	// シェーダーリソースビュー.
	std::vector<ID3D11Texture2D*>			m_pTexture2DList;			// テクスチャ2D.

	ID3D11VertexShader*		m_pVertexShader;		// 頂点シェーダー.
	ID3D11PixelShader*		m_pPixelShader;			// ピクセルシェーダー.
	ID3D11InputLayout*		m_pVertexLayout;		// 頂点レイアウト.
	ID3D11SamplerState*		m_pSampleLinear;		// サンプラ.

	ID3D11Buffer*			m_pConstantBufferInit;	// コンスタントバッファ.
	ID3D11Buffer*			m_pVertexBuffer;		// 頂点バッファ.

	UINT					m_WndWidth;			// ウィンドウ幅.
	UINT					m_WndHeight;		// ウィンドウ高さ.
};

#endif	// #ifndef RENDER_TEXTURE_H.