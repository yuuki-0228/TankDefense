/**
* @file DebugText.h.
* @brief デバッグテキスト描画クラス.
* @author 福田玲也.
*/
#ifndef DEBUG_TEXT_H
#define DEBUG_TEXT_H

#include <queue>
#include <string>
#include <sstream>

#include "..\Common.h"

class CDebugTextShader;
class CFont;
class CSpriteRender;

/*********************************
*	デバッグテキスト描画クラス.
**/
class CDebugText : public CCommon
{
private:
	static constexpr size_t	MAX_TEXT_LENGH	= 70;	// 最大文字数.

public:
	CDebugText();	// コンストラクタ
	virtual ~CDebugText();	// デストラクタ.

	// インスタンスの取得.
	static CDebugText* GetInstance();

	// 初期化関数.
	static HRESULT Init( ID3D11DeviceContext* pContext11 );

	// 描画関数(可変引数).
	template<class... T1>
	static void PushText( const T1&... t1 )
	{
		if( GetInstance()->m_IsRender			== false ) return;
		std::string Text = GetInstance()->initstring(t1...);
		if( Text.length() >= GetInstance()->MAX_TEXT_LENGH ) return;
		GetInstance()->m_TextQueue.push( Text );
	}
	// 描画関数.
	static void Render();

private:
	// 更新関数.
	void Update();
	// テクスチャの読み込み.
	void TextureLoad();

	template <class T>
	std::string to_std_string(const T& x)
	{
		std::stringstream ss;
		ss << x;
		return ss.str();
	}
	template<class T1, class... T2>
	std::string initstring( const T1& t1, const T2&... t2 )
	{
		std::string s = to_std_string(t1);
		return s + initstring(t2...);
	}
	template<class T1>
	std::string initstring( const T1& t1 )
	{
		std::string s = to_std_string(t1);
		return s;
	}

private:
	std::queue<std::string>			m_TextQueue;		// テキストキュー.
	std::unique_ptr<CFont>			m_pFont;			// フォント.
	std::unique_ptr<CSpriteRender>	m_pSpriteRender;	// 画像描画クラス.
	CSprite*						m_pBackSprite;		// 背景画像.
	D3DXVECTOR2						m_BackTextureSize;	// 背景の画像サイズ.
	int								m_TextIndex;		// テキストインデックス.
	bool							m_IsRender;			// 描画できるか.
	bool							m_IsMoveing;		// 移動できるか.

private:
	// コピー・ムーブコンストラクタ, 代入演算子の削除.
	CDebugText( const CDebugText & )				= delete;
	CDebugText& operator = ( const CDebugText & )	= delete;
	CDebugText( CDebugText && )						= delete;
	CDebugText& operator = ( CDebugText && )		= delete;
};

#endif // #ifndef DEBUG_TEXT_H.