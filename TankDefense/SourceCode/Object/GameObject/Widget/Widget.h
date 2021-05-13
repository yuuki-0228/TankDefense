/**
* @file Widget.h.
* @brief ゲーム内UI基底クラス.
* @author 福田玲也.
*/
#ifndef WIDGET_H
#define WIDGET_H

#include "..\GameObject.h"
#include "..\..\..\Common\Sprite\Sprite.h"

/**************************************
*	ゲーム内のUI基底クラス.
**/
class CWidget : public CGameObject
{
protected:
	// フェードの状態.
	enum class enFadeState
	{
		In,			// フェードイン.
		Out,		// フェードアウト.
		Finish,		// フェード終了.
	} typedef EFadeState;

protected:
	static constexpr float ALPHA_MAX = 1.0f;	// アルファ最大値.
	static constexpr float ALPHA_MIN = 0.0f;	// アルファ最小値.

public:
	CWidget();
	virtual ~CWidget();

	// フェードインの設定.
	void SetFadeIn( const float& speedSec );
	// フェードアウトの設定.
	void SetFadeOut( const float& speedSec );

protected:
	// 画像の設定.
	virtual bool SpriteSetting( const char* sprite, const int& stateNum = 1 );

	// フェードの更新.
	void FadeUpdate( float& alpha );
	// フェードイン.
	void FadeIn( float& alpha );
	// フェードアウト.
	void FadeOut( float& alpha );

private:
	// アルファの計算.
	void AlphaCalc( float& alpha );

protected:
	CSprite*						m_pSprite;		// 画像クラス.
	std::vector<SSpriteRenderState>	m_RenderStates;	// 画像描画ステートリスト.
	float							m_Alpha;		// アルファ値.
	float							m_FafeSpeed;	// フェード速度.
	EFadeState						m_NowFadeState;	// 現在のフェード状態.
	EFadeState						m_OldFadeState;	// 過去のフェード状態.
};

#endif	// #ifndef WIDGET_H.
