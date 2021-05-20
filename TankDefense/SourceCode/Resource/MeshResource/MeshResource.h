/**
* @file MeshResource.h.
* @brief メッシュリソースクラス.
* @author 福田玲也.
*/
#ifndef MESH_RESORCE_H
#define MESH_RESORCE_H

#include "..\..\Global.h"

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <filesystem>	// C++17 必須.

namespace fs = std::filesystem;

class CDX9SkinMesh;
class CDX9StaticMesh;

class CMeshResorce
{
	const char* FILE_PATH = "Data\\Mesh";	// メッシュファイルパス.

	typedef std::unordered_map<std::string, std::unique_ptr<CDX9StaticMesh>>	static_mesh_map;
	typedef std::unordered_map<std::string, std::unique_ptr<CDX9SkinMesh>>		skin_mesh_map;

public:
	CMeshResorce();
	~CMeshResorce();

	// インスタンスの取得.
	static CMeshResorce* GetInstance();

	// モデルの読み込み(ラッパー).
	static HRESULT Load( HWND hWnd, ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11, LPDIRECT3DDEVICE9 pDevice9 );

	// スタティックメッシュの読み込み.
	static CDX9StaticMesh* GetStatic( const std::string& name );
	// スキンメッシュの読み込み.
	static CDX9SkinMesh* GetSkin( const std::string& name );

private:
	// モデルの読み込み.
	HRESULT ModelLoad( HWND hWnd, ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11, LPDIRECT3DDEVICE9 pDevice9 );

private:
	static_mesh_map m_StaticMeshList;	// スタティックメッシュのリスト.
	skin_mesh_map	m_SkinMeshList;		// スキンメッシュのリスト.
	bool m_HasFinishedLoading;			// 読込が終わったか.

private:
	// コピー・ムーブコンストラクタ, 代入演算子の削除.
	CMeshResorce( const CMeshResorce & )				= delete;
	CMeshResorce& operator = ( const CMeshResorce & )	= delete;
	CMeshResorce( CMeshResorce && )						= delete;
	CMeshResorce& operator = ( CMeshResorce && )		= delete;
};

namespace mesh
{
	// スキンメッシュのアタッチ.
	template<class T = CDX9SkinMesh>
	static bool AttachSkinMesh( T** pMesh, const char* name )
	{
		*pMesh = CMeshResorce::GetSkin( name );
		if( *pMesh == nullptr ) return false;
		return true;
	}

	// スタティックメッシュのアタッチ.
	template<class T = CDX9StaticMesh>
	static bool AttachStaticMesh( T** pMesh, const char* name )
	{
		*pMesh = CMeshResorce::GetStatic( name );
		if( *pMesh == nullptr ) return false;
		return true;
	}
};

#endif	// #ifndef MESH_RESORCE_H.