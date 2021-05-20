/**
* @file MeshResource.h.
* @brief ���b�V�����\�[�X�N���X.
* @author ���c���.
*/
#ifndef MESH_RESORCE_H
#define MESH_RESORCE_H

#include "..\..\Global.h"

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <filesystem>	// C++17 �K�{.

namespace fs = std::filesystem;

class CDX9SkinMesh;
class CDX9StaticMesh;

class CMeshResorce
{
	const char* FILE_PATH = "Data\\Mesh";	// ���b�V���t�@�C���p�X.

	typedef std::unordered_map<std::string, std::unique_ptr<CDX9StaticMesh>>	static_mesh_map;
	typedef std::unordered_map<std::string, std::unique_ptr<CDX9SkinMesh>>		skin_mesh_map;

public:
	CMeshResorce();
	~CMeshResorce();

	// �C���X�^���X�̎擾.
	static CMeshResorce* GetInstance();

	// ���f���̓ǂݍ���(���b�p�[).
	static HRESULT Load( HWND hWnd, ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11, LPDIRECT3DDEVICE9 pDevice9 );

	// �X�^�e�B�b�N���b�V���̓ǂݍ���.
	static CDX9StaticMesh* GetStatic( const std::string& name );
	// �X�L�����b�V���̓ǂݍ���.
	static CDX9SkinMesh* GetSkin( const std::string& name );

private:
	// ���f���̓ǂݍ���.
	HRESULT ModelLoad( HWND hWnd, ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11, LPDIRECT3DDEVICE9 pDevice9 );

private:
	static_mesh_map m_StaticMeshList;	// �X�^�e�B�b�N���b�V���̃��X�g.
	skin_mesh_map	m_SkinMeshList;		// �X�L�����b�V���̃��X�g.
	bool m_HasFinishedLoading;			// �Ǎ����I�������.

private:
	// �R�s�[�E���[�u�R���X�g���N�^, ������Z�q�̍폜.
	CMeshResorce( const CMeshResorce & )				= delete;
	CMeshResorce& operator = ( const CMeshResorce & )	= delete;
	CMeshResorce( CMeshResorce && )						= delete;
	CMeshResorce& operator = ( CMeshResorce && )		= delete;
};

namespace mesh
{
	// �X�L�����b�V���̃A�^�b�`.
	template<class T = CDX9SkinMesh>
	static bool AttachSkinMesh( T** pMesh, const char* name )
	{
		*pMesh = CMeshResorce::GetSkin( name );
		if( *pMesh == nullptr ) return false;
		return true;
	}

	// �X�^�e�B�b�N���b�V���̃A�^�b�`.
	template<class T = CDX9StaticMesh>
	static bool AttachStaticMesh( T** pMesh, const char* name )
	{
		*pMesh = CMeshResorce::GetStatic( name );
		if( *pMesh == nullptr ) return false;
		return true;
	}
};

#endif	// #ifndef MESH_RESORCE_H.