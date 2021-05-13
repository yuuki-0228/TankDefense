/**
* @file Font.h.
* @brief フォント描画クラス.
* @author 福田玲也.
*/
#ifndef FONT_H
#define FONT_H

#include "..\Common.h"

class CSpriteRender;

/********************************************
*	フォント描画クラス.
**/
class CFont : public CCommon
{
public:
	const float FONT_SIZE = 25.0f;	// フォントの大きさ.
	const D3DXVECTOR2 TEXTUR_UV_POS = { 1.0f, 1.0f };	// テクスチャUV座標.

public:
	CFont();
	virtual ~CFont();

	// 初期化.
	HRESULT Init( ID3D11DeviceContext* pContext11 );
	// 解放.
	void Release();

	// UIで描画.
	void RenderUI( const std::string& text );
	// 3Dで描画.
	void Render3D( const std::string& text, const bool& isBillboard = false );

private:
	// テクスチャの描画.
	void RenderFontUI( const char* c );
	// テクスチャの描画.
	void RenderFont3D( const char* c, const bool& isBillboard  );
	// バーテックスバッファの作成.
	HRESULT CreateVerTexBuffer();

private:
	std::unique_ptr<CSpriteRender>	m_pSpriteRender;	// 画像描画.
	ID3D11Buffer*					m_pVertexBuffer3D;	// バーテックスバッファ.
	ID3D11Buffer*					m_pVertexBufferUI;	// 頂点バッファ.
};

#endif	// #ifndef FONT_H.