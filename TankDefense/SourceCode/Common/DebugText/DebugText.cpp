#include "DebugText.h"
#include "..\Font\Font.h"
#include "..\Sprite\SpriteRender.h"
#include "..\Sprite\Sprite.h"
#include "..\..\Resource\SpriteResource\SpriteResource.h"
#include "..\..\Utility\Input\Input.h"

namespace
{
	constexpr	int		INDEX_SUB_MAX	= 28;	// 表示行サイズ.
	constexpr	float	RENDER_SIZE		= 0.4f;	// 描画サイズ.
	const D3DXVECTOR4	RENDER_COLOR	= { 0.9f, 0.2f, 0.2f, 1.0f };	// 描画色.
};

CDebugText::CDebugText()
	: m_TextQueue		()
	, m_pFont			( nullptr )
	, m_pSpriteRender	( nullptr )
	, m_pBackSprite		( nullptr )
	, m_BackTextureSize	( 0.0f, 0.0f )
	, m_TextIndex		( 0 )
#ifdef _DEBUG
	, m_IsRender		( true )
#else
	, m_IsRender		( false )
#endif	// #ifdef _DEBUG.
	, m_IsMoveing		( false )
{
	m_pFont = std::make_unique<CFont>();
	m_pSpriteRender = std::make_unique<CSpriteRender>();
}

CDebugText::~CDebugText()
{
}

//----------------------------.
// インスタンスの取得.
//----------------------------.
CDebugText* CDebugText::GetInstance()
{
	static std::unique_ptr<CDebugText> pInstance = std::make_unique<CDebugText>();	// インスタンスの作成.
	return pInstance.get();
}

//----------------------------.
// 初期化関数.
//----------------------------.
HRESULT CDebugText::Init( ID3D11DeviceContext* pContext11 )
{
	if( FAILED( GetInstance()->SetDevice( pContext11 ) ))		return E_FAIL;
	if( FAILED( GetInstance()->m_pFont->Init( pContext11 ) ))	return E_FAIL;
	if( FAILED( GetInstance()->m_pSpriteRender->Init( pContext11 ) ))	return E_FAIL;

	GetInstance()->m_pFont->SetScale( RENDER_SIZE );
	GetInstance()->m_pFont->SetColor( RENDER_COLOR );

	return S_OK;
}

//----------------------------.
// 描画関数.
//----------------------------.
void CDebugText::Render()
{
	// デバッグテキストの描画切り替え.
	if( CKeyInput::IsPress(VK_LCONTROL) && CKeyInput::IsMomentPress(VK_F7) ) GetInstance()->m_IsRender = !GetInstance()->m_IsRender;

	GetInstance()->TextureLoad();

	if( GetInstance()->m_IsRender			== false ) return;
	if( GetInstance()->m_pSpriteRender		== nullptr ) return;
	if( GetInstance()->m_pBackSprite		== nullptr ) return;
	if( GetInstance()->m_TextQueue.empty()	== true ) return;

	GetInstance()->Update();	// 更新.

	SSpriteRenderState ss;
	ss.Tranceform = GetInstance()->m_Tranceform;
	GetInstance()->m_pSpriteRender->Render( ss, GetInstance()->m_pBackSprite->GetTexture(), GetInstance()->m_pBackSprite->GetVertexBufferUI() );

	// 描画座標を取得.
	D3DXVECTOR3 pos = GetInstance()->m_Tranceform.Position;
	pos.x += (GetInstance()->m_pFont->FONT_SIZE*0.5f * GetInstance()->m_Tranceform.Scale.x);

	// キューのサイズを取得.
	const int queueSize = static_cast<int>(GetInstance()->m_TextQueue.size());
	for( int i = 0; i < queueSize; i++ ){
		// テキストのインデックスが有効範囲外なら終了.
		if( i < GetInstance()->m_TextIndex ){
			GetInstance()->m_TextQueue.pop();
			continue;
		}

		// 座標を下に下ろす.
		pos.y += (GetInstance()->m_pFont->FONT_SIZE*0.5f * GetInstance()->m_Tranceform.Scale.x);

		// テキストが背景外なら終了.
		if( pos.y < GetInstance()->m_Tranceform.Position.y+GetInstance()->m_BackTextureSize.y ){
			GetInstance()->m_pFont->SetPosition( pos );
			GetInstance()->m_pFont->RenderUI( GetInstance()->m_TextQueue.front() );
		}

		GetInstance()->m_TextQueue.pop();

	}

}

//----------------------------.
// 更新関数.
//----------------------------.
void CDebugText::Update()
{
	// マウスが無効なら終了.
	if( CInput::IsScreenMiddleMouse() == false ) return;

	// マウス座標が画面外なら終了.
	POINT mousePos = CInput::GetMousePosition();
	if( m_Tranceform.Position.x > mousePos.x || mousePos.x > m_Tranceform.Position.x+m_BackTextureSize.x ||
		m_Tranceform.Position.y > mousePos.y || mousePos.y > m_Tranceform.Position.y+m_BackTextureSize.y ){
		m_IsMoveing = false;
		CInput::SetMouseWheelDelta( 0 );	// ホイール値を初期化.
		return;
	}

	// テキストの表示位置を変更する.
	int mouseDelta = CInput::GetMouseWheelDelta();		// ホイール値を取得.
	if( mouseDelta < 0 ) GetInstance()->m_TextIndex++;	// 下方向へ移動.
	if( mouseDelta > 0 ) GetInstance()->m_TextIndex--;	// 上方向へ移動.

	// スクロール範囲を設定.
	const int queueSize = static_cast<int>(GetInstance()->m_TextQueue.size());
	const int indexMax =  queueSize > INDEX_SUB_MAX ? abs(INDEX_SUB_MAX-queueSize) : queueSize-1;

	// 値の範囲外の調整.
	if( GetInstance()->m_TextIndex > indexMax ) GetInstance()->m_TextIndex = indexMax;
	if( GetInstance()->m_TextIndex < 0 ) GetInstance()->m_TextIndex = 0;
	CInput::SetMouseWheelDelta( 0 );	// ホイール値を初期化.

	if( CKeyInput::IsMomentPress(VK_LBUTTON) == true ) m_IsMoveing = true;
	if( CKeyInput::IsRelease(VK_LBUTTON) == true ){
		m_IsMoveing = false;
		return;
	}

	if( m_IsMoveing == false ) return;

	// 前回の座標と今の座標の差分描画座標に足す.
	float ySub = CInput::GetMousePosisionY() - CInput::GetMouseOldPosisionY();
	float xSub = CInput::GetMousePosisionX() - CInput::GetMouseOldPosisionX();

	m_Tranceform.Position.x += xSub;
	m_Tranceform.Position.y += ySub;
}

// テクスチャの読み込み.
void CDebugText::TextureLoad()
{
	if( GetInstance()->m_pBackSprite != nullptr ) return;
	GetInstance()->m_pBackSprite = CSpriteResource::GetSprite( "debugBack" );
	if( GetInstance()->m_pBackSprite == nullptr ) return;

	GetInstance()->m_BackTextureSize.x = GetInstance()->m_pBackSprite->GetTextureSize().w;
	GetInstance()->m_BackTextureSize.y = GetInstance()->m_pBackSprite->GetTextureSize().h;
	GetInstance()->m_Tranceform.Position = GetInstance()->m_pBackSprite->GetRenderPos();
	if( GetInstance()->m_BackTextureSize.x == 0.0f ) GetInstance()->m_pBackSprite = nullptr;
}