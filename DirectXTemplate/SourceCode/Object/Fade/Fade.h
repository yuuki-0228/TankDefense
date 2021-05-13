/**
* @file Fade.h.
* @brief シーンが切り替わるタイミングでフェードを描画させるクラス.
* @author 福田玲也.
*/
#ifndef FADE_H
#define FADE_H

#include "..\Object.h"
#include "..\..\Common\Sprite\SpriteStruct.h"

/**********************************
*	フェードクラス.
**/
class CFade : public CObject
{
	enum class enFadeState
	{
		None,

		In,		// フェードイン.
		Out,	// フェードアウト.
		End,	// フェード終了.
	} typedef EFadeState;
public:
	CFade();
	virtual ~CFade();

	// インスタンスの生成.
	static CFade* GetInstance();

	// 初期化.
	static bool Init();

	// 更新.
	static void Update( const float& deltaTime );
	// 描画.
	static void Render();

	// 現在の状態を取得.
	static EFadeState GetNowState(){ return GetInstance()->m_NowState; }

	// フェードイン設定関数.
	static bool IsFadeIn(){ return GetInstance()->m_NowState == EFadeState::In; }
	// フェードアウト設定関数.
	static bool IsFadeOut(){ return GetInstance()->m_NowState == EFadeState::Out; }
	// フェードアウト設定関数.
	static bool IsFadeEnd(){ return GetInstance()->m_NowState == EFadeState::End; }

	// フェードイン設定関数.
	static void SetFadeIn( const bool& isNextFade = false );
	// フェードアウト設定関数.
	static void SetFadeOut( const bool& isNextFade = false );

	// 続けて描画する.
	static void KeepRender( const bool& isKeep = true );

private:
	// 画像の読み込み.
	bool LoadSprite();

private:
	std::unique_ptr<CSprite>	m_MainSprite;		// メインの画像.
	std::unique_ptr<CSprite>	m_MaskSprite;		// マスク画像.
	SSpriteRenderState			m_MainSpriteState;	// メイン画像の情報.
	EFadeState					m_NowState;			// 現在の状態.
	bool						m_IsNextFade;		// 続けてフェード処理するか.
	bool						m_IsKeepRender;		// 続けて描画するか.

private:
	//コピー・ムーブコンストラクタ、代入演算子の削除.
	CFade(const CFade &)				= delete;
	CFade(CFade &&)						= delete;
	CFade& operator = (const CFade &)	= delete;
	CFade& operator =(CFade &&)			= delete;
};

#endif	// #ifndef FADE_H.