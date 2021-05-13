/**
* @file FontResource.h.
* @brief フォントトリソースクラス.
* @author 福田玲也.
*/
#ifndef FONT_RESOURCE_H
#define FONT_RESOURCE_H

#include "..\..\Global.h"

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <filesystem>	// C++17 必須.

namespace fs = std::filesystem;

/***********************************
*	フォントリソースクラス.
**/
class CFontResource
{
	const char* TEXT_LIST_PATH = "Data\\TextList\\TextList.txt";
public:
	CFontResource();
	~CFontResource();

	// インスタンスの取得.
	static CFontResource* GetInstance();
	// フォントの読み込み.
	static void Load( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11 );
	// フォントテクスチャの読み込み.
	static ID3D11ShaderResourceView* GetTexture2D( const std::string key );

private:
	// フォントの読み込み.
	void FontLoad( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11 );

private:
	std::unordered_map<std::string, ID3D11ShaderResourceView*> m_FontTexturList;
	bool m_HasFinishedLoading;			// 読込が終わったか.
};

#endif	// #ifndef FONT_RESOURCE_H.