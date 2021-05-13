#ifndef RAY_MESH_H
#define RAY_MESH_H

#include "..\..\Common.h"

class CRayMesh : public CCommon
{
	// 頂点の構造体.
	struct VERTEX
	{
		D3DXVECTOR3 Pos;	// 頂点座標.
	};
	// コンスタントバッファ.
	struct C_BUFFER_PER_FRAME
	{
		D3DXMATRIX	mWVP;	// ワールド行列.
		D3DXVECTOR4	vColor;	// 色.
	};
public:
	CRayMesh();
	virtual ~CRayMesh();

	// 初期化関数.
	HRESULT Init( ID3D11DeviceContext* pContext11 );
	// 解放.
	void Release();

	// 描画関数.
	void Render();

	// ベクトルの設定.
	void SetVector( const D3DXVECTOR3& v ){ m_Vector = v; }

private:
	// シェーダーの作成.
	HRESULT InitShader();
	// 頂点バッファの作成.
	HRESULT InitVertexBuffer();

private:
	ID3D11VertexShader*		m_pVertexShader;	// 頂点シェーダー.
	ID3D11PixelShader*		m_pPixelShader;		// ピクセルシェーダー.
	ID3D11InputLayout*		m_pVertexLayout;	// 頂点レイアウト.
	ID3D11Buffer*			m_pConstantBuffer;	// コンスタントバッファ.
	ID3D11Buffer*			m_pVertexBuffer;	// 頂点バッファ.

	D3DXVECTOR3				m_Vector;			// 方向ベクトル.
};

#endif	// #ifndef RAY_MESH_H.