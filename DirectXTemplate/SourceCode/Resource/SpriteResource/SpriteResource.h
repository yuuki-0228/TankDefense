/**
* @file SpriteResource.h.
* @brief 画像リソースクラス.
* @author 福田玲也.
*/
#ifndef SPRITE_RESOURCE_H
#define SPRITE_RESOURCE_H

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>	// C++17 必須.

#include "..\..\Common\Sprite\Sprite.h"

namespace fs = std::filesystem;

class CSpriteResource
{
	const char* FILE_PATH = "Data\\Sprite";	// スプライトフォルダパス.
	typedef std::unordered_map<std::string, std::unique_ptr<CSprite>> sprite_map;
public:
	CSpriteResource();
	~CSpriteResource();

	// インスタンスの取得.
	static CSpriteResource* GetInstance();

	// スプライトの読み込み(ラッパー).
	static HRESULT Load( ID3D11DeviceContext* pContext11 );

	// スプライト取得関数.
	static CSprite* GetSprite( const std::string& spriteName );

private:
	// スプライト読み込み.
	HRESULT SpriteLoad( ID3D11DeviceContext* pContext11 );


private:
	sprite_map m_SpriteList;	// スプライトリスト.
	bool m_HasFinishedLoading;	// 読込が終わったか.

private:
	// コピー・ムーブコンストラクタ, 代入演算子の削除.
	CSpriteResource( const CSpriteResource & )				= delete;
	CSpriteResource& operator = ( const CSpriteResource & )	= delete;
	CSpriteResource( CSpriteResource && )					= delete;
	CSpriteResource& operator = ( CSpriteResource && )		= delete;
};

#endif	// #ifndef SPRITE_RESOURCE_H.