#include "FontResource.h"
#include "..\..\Common\Font\FontCreate.h"
#include "..\..\Utility\FileManager\FileManager.h"

CFontResource::CFontResource()
	: m_HasFinishedLoading	( false )
{
}

CFontResource::~CFontResource()
{
	for( auto m : m_FontTexturList ){
		SAFE_RELEASE(m.second);
	}
}

//-------------------------------------.
// インスタンスの取得.
//-------------------------------------.
CFontResource* CFontResource::GetInstance()
{
	static std::unique_ptr<CFontResource> pInstance =
		std::make_unique<CFontResource>();
	return pInstance.get();
}

//-------------------------------------.
// フォントの読み込み.
//-------------------------------------.
void CFontResource::Load( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11 )
{
	GetInstance()->FontLoad( pDevice11, pContext11 );
}

//-------------------------------------.
// フォントテクスチャの読み込み.
//-------------------------------------.
ID3D11ShaderResourceView* CFontResource::GetTexture2D( const std::string key )
{
	// 指定したモデルを返す.
	for( auto& m : GetInstance()->m_FontTexturList ){
		if( m.first == key ) return m.second;
	}

	// モデルの読込が終了してないので null を返す.
	if( GetInstance()->m_HasFinishedLoading == false ) return nullptr;

	ERROR_MESSAGE( key + " key not found" );
	return nullptr;
}

//-------------------------------------.
// フォントの読み込み.
//-------------------------------------.
void CFontResource::FontLoad( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11 )
{
	std::unique_ptr<CFontCreate> pFontCreate = std::make_unique<CFontCreate>( pDevice11, pContext11 );
	std::vector<std::string> textList = fileManager::TextLoading(TEXT_LIST_PATH, false);

	for( const auto& v : textList ){
		for( int i = 0; i < static_cast<int>(v.size()); i++ ){
			std::string f = v.substr( i, 1 );
			if( IsDBCSLeadByte( v[i] ) == TRUE ){
				f = v.substr( i++, 2 );	// 全角文字.
			} else {
				f = v.substr( i, 1 );	// 半角文字.
			}
			// mapにすでに同じkeyがあればコンテニュー.
			bool keyFind = (m_FontTexturList.find(f) != m_FontTexturList.end());
			if( keyFind == true ) continue;

			// テクスチャの作成.
			pFontCreate->CreateFontTexture2D( f.c_str(), &m_FontTexturList[f] );
		}
	}

	// 読込が終わったので true にする.
	m_HasFinishedLoading = true;
}