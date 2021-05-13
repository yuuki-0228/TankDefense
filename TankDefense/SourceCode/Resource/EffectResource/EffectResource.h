/**
* @file EffectResource.h.
* @brief エフェクトリソースクラス.
* @author 福田玲也.
*/
#ifndef EFFECT_RESOURCE_H
#define EFFECT_RESOURCE_H

#include "..\..\Global.h"

#include <unordered_map>
#include <string>
#include <memory>
#include <filesystem>

namespace fs = std::filesystem;

class CEffect;

/**********************************************
*	エフェクトリソースクラス (シングルトン).
*/
class CEffectResource
{
	const char* FILE_PATH = "Data\\Effect";	// エフェクトファイルパス.
	typedef std::unordered_map<std::string, std::unique_ptr<CEffect>> effect_map;
public:
	CEffectResource();
	~CEffectResource();
	// インスタンスの取得.
	static CEffectResource* GetInstance();

	// 読み込み(ラッパー).
	static void Load( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11 );
	// 解放.
	static void Release();
	// エフェクト取得関数.
	static CEffect* GetEffect( const std::string& fileName );

private:
	// エフェクトの読み込み.
	void EffectLoad( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11 );

private:
	effect_map m_EffectList;	// エフェクトリスト.
	bool m_HasFinishedLoading;	// 読込が終わったか.

private:
	// コピー・ムーブコンストラクタ, 代入演算子の削除.
	CEffectResource( const CEffectResource & )					= delete;
	CEffectResource& operator = ( const CEffectResource & )		= delete;
	CEffectResource( CEffectResource && )						= delete;
	CEffectResource& operator = ( CEffectResource && )			= delete;
};

#endif	// #ifndef EFFECT_RESOURCE_H.