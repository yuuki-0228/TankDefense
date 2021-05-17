/**
* @file Mesh.h.
* @brief 当たり判定用メッシュクラス.
* @author 福田玲也.
*/
#ifndef MESH_H
#define MESH_H

#include "..\ColliderBase.h"


/*****************************************
*	メッシュクラス(当たり判定用).
**/
class CMesh : public CColliderBase
{
public:
	CMesh();
	virtual ~CMesh();

	// メッシュの取得.
	inline LPD3DXMESH	GetMesh() const				{ return m_pMeshForRay; }
	// メッシュの設定.
	inline void			SetMesh( LPD3DXMESH pMesh )	{ m_pMeshForRay = pMesh; }

	// 交差位置のポリゴンの頂点を見つける.
	HRESULT FindVerticesOnPoly( const DWORD dwPolyIndex, D3DXVECTOR3* pOutVecVertices );

private:
	LPD3DXMESH	m_pMeshForRay;	// レイとの当たり判定用メッシュオブジェクト.
};

#endif	// #ifndef MESH_H.
