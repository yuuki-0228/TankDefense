#include "Widget.h"
#include "..\..\..\Resource\SpriteResource\SpriteResource.h"

CWidget::CWidget()
	: m_pSprite			( nullptr )
	, m_RenderStates	()
	, m_Alpha			( ALPHA_MAX )
	, m_FafeSpeed		( FOR_INIT_ZERO_FLOAT )
	, m_NowFadeState	( EFadeState::Finish )
	, m_OldFadeState	( EFadeState::Finish )
{
}

CWidget::~CWidget()
{
	m_pSprite = nullptr;
}

//--------------------------------------.
// フェードインの設定.
//--------------------------------------.
void CWidget::SetFadeIn( const float& speed )
{
	m_NowFadeState = EFadeState::In;
	m_FafeSpeed = speed;
}

//--------------------------------------.
// フェードアウトの設定.
//--------------------------------------.
void CWidget::SetFadeOut( const float& speed )
{
	m_NowFadeState = EFadeState::Out;
	m_FafeSpeed = speed;
}

//--------------------------------------.
// 画像の設定.
//--------------------------------------.
bool CWidget::SpriteSetting( const char* spriteName, const int& stateNum )
{
	// スプライトの取得.
	m_pSprite = CSpriteResource::GetSprite( spriteName );
	if( m_pSprite == nullptr ) return false;

	// スプライト描画ステートを描画数分取得.
	m_RenderStates.resize( stateNum, m_pSprite->GetRenderState() );

	return true;
}

//--------------------------------------.
// フェードの更新.
//--------------------------------------.
void CWidget::FadeUpdate( float& alpha )
{
	switch( m_NowFadeState )
	{
	case EFadeState::In:
		FadeIn( alpha );
		break;
	case EFadeState::Out:
		FadeOut( alpha );
		break;
	default:
		break;
	}
}

//--------------------------------------.
// フェードイン.
//--------------------------------------.
void CWidget::FadeIn( float& alpha )
{
	AlphaCalc( alpha );
	if( m_Alpha <= ALPHA_MIN ){
		m_Alpha = ALPHA_MIN;
		m_FafeSpeed = FOR_INIT_ZERO_FLOAT;
		m_OldFadeState = m_NowFadeState;
		m_NowFadeState = EFadeState::Finish;
	}
}

//--------------------------------------.
// フェードアウト.
//--------------------------------------.
void CWidget::FadeOut( float& alpha )
{
	AlphaCalc( alpha );
	if( m_Alpha >= ALPHA_MAX ){
		m_Alpha = ALPHA_MAX;
		m_FafeSpeed = FOR_INIT_ZERO_FLOAT;
		m_OldFadeState = m_NowFadeState;
		m_NowFadeState = EFadeState::Finish;
	}
}

//--------------------------------------.
// アルファの計算.
//--------------------------------------.
void CWidget::AlphaCalc( float& alpha )
{
	alpha += m_DeltaTime / m_FafeSpeed;
}