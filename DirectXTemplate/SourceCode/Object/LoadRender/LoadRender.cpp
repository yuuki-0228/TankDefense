#include "LoadRender.h"
#include "..\..\Common\Sprite\Sprite.h"
#include "..\..\Common\Sprite\SpriteRender.h"

namespace
{
	constexpr char	BACK_SPRITE_NAME[]			= "Data\\Sprite\\loadBase.png";	// 背景の画像.
	constexpr float	BACK_SPRITE_SCROLL_SPEED	= 0.1f;
	enum enSpriteNo : unsigned char
	{
		ESpriteNo_None,

		ESpriteNo_Back = 0,

	} typedef ESpriteNo;
};


CLoadRender::CLoadRender()
	: m_pSprites		()
	, m_DeltaTime		( 0.0f )
	, m_IsLoadEnd		( false )
{
}

CLoadRender::~CLoadRender()
{
}

// 読み込みクラス.
bool CLoadRender::Init()
{
	if( m_IsLoadEnd == true ) return true;

	if( LoadSprite()	== false )		return false;

	m_IsLoadEnd = true;
	return true;
}

// 更新関数.
void CLoadRender::Update( const float& deltaTime )
{
	m_DeltaTime = deltaTime;
}

// 描画クラス.
void CLoadRender::Render()
{
	if( m_IsLoadEnd == false ) return;

	int spiteNo = ESpriteNo_None;
	for( auto& s : m_pSprites ){
		switch( spiteNo )
		{
		case ESpriteNo_Back:
			s->SetScrollSpeedX( BACK_SPRITE_SCROLL_SPEED * m_DeltaTime );
			break;
		}
		s->Render();
		spiteNo++;
	}
}

// スプライトの設定.
bool CLoadRender::LoadSprite()
{
	const char* loadSpriteList[] =
	{
		BACK_SPRITE_NAME,
	};

	int spiteNo = ESpriteNo_None;
	// 読み込むスプライトの数文ループ.
	for( auto& s : loadSpriteList ){
		m_pSprites.emplace_back( std::make_unique<CSprite>() );	// 要素の追加.
		// 画像の読み込み.
		if( FAILED( m_pSprites.back()->Init( CDirectX11::GetContext(), s ) )){
			// 失敗した場合、要素数を0にして終了.
			m_pSprites.clear();
			return false;
		}

		switch( spiteNo )
		{
		case ESpriteNo_Back:
			m_pSprites.back()->SetScrollUpdateX( true );
			break;
		}

		spiteNo++;
	}
	return true;
}