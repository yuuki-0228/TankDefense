#include "Mesh.h"

CMesh::CMesh()
	: m_pMeshForRay	( nullptr )
{
}

CMesh::~CMesh()
{
	m_pMeshForRay = nullptr;
}

// 交差位置のポリゴンの頂点を見つける.
HRESULT CMesh::FindVerticesOnPoly( const DWORD dwPolyIndex, D3DXVECTOR3* pOutVecVertices )
{
	// 頂点ごとのバイト数を取得.
	DWORD dwStrid		= m_pMeshForRay->GetNumBytesPerVertex();
	// 頂点数を取得.
	DWORD dwVertexAmt	= m_pMeshForRay->GetNumVertices();
	// 面数を取得.
	DWORD dwPolyAmt		= m_pMeshForRay->GetNumFaces();

	WORD* pwPoly = nullptr;

	// インデックスバッファをロック(読み込みモード).
	m_pMeshForRay->LockIndexBuffer( D3DLOCK_READONLY, (VOID**)&pwPoly );
	BYTE* pbVertices	= nullptr;	// 頂点(バイト数).
	FLOAT* pfVerTices	= nullptr;	// 頂点(FLOAT型).
	LPDIRECT3DVERTEXBUFFER9 VB = nullptr;	// 頂点バッファ.
	m_pMeshForRay->GetVertexBuffer( &VB );	// 頂点情報の取得.

	// 頂点バッファのロック.
	if( SUCCEEDED( VB->Lock( 0, 0, (VOID**)&pbVertices, 0 ) ) ){
		for( int i = 0; i < 3; i++ ){
			// ポリゴンの頂点の3つ目を取得.
			pfVerTices
				= (FLOAT*)( &pbVertices[dwStrid*pwPoly[dwPolyIndex * 3 + i]] );
			pOutVecVertices[i].x = pfVerTices[0];
			pOutVecVertices[i].y = pfVerTices[1];
			pOutVecVertices[i].z = pfVerTices[2];
		}

		m_pMeshForRay->UnlockIndexBuffer();
		VB->Unlock();
	}
	SAFE_RELEASE( VB );

	return S_OK;
}