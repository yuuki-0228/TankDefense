#ifndef DX9_STATIC_MESH_H
#define DX9_STATIC_MESH_H

#include "..\..\Common.h"
#include "..\MeshStruct.h"

/*************************************************
*	スタティックメッシュ.
**/
class CDX9StaticMesh : public CCommon
{
	// 頂点の構造体.
	struct VERTEX
	{
		D3DXVECTOR3 Pos;		// 頂点座標.
		D3DXVECTOR3	Normal;		// 法線(陰影計算に必須).
		D3DXVECTOR2	Tex;		// テクスチャ座標.
		D3DXVECTOR3	Tangent;	// .
		D3DXVECTOR3	Binormal;	// 法線(陰影計算に必須).
	};
	// マテリアル構造体.
	struct MY_MATERIAL
	{
		D3DXVECTOR4	Ambient;					// アンビエント.
		D3DXVECTOR4	Diffuse;					// ディフューズ.
		D3DXVECTOR4	Specular;					// スペキュラ.
		char szTextureName[64];					// テクスチャファイル名.
		ID3D11ShaderResourceView* pTexture;		// テクスチャ.
		ID3D11ShaderResourceView* pNormTexture;	// 法線テクスチャ.
		DWORD dwNumFace;						// そのマテリアルのポリゴン数.

		MY_MATERIAL()
			: Ambient		()
			, Diffuse		()
			, Specular		()
			, szTextureName	()
			, pTexture		( nullptr )
			, pNormTexture	( nullptr )
			, dwNumFace		( 0)
		{
		}

		~MY_MATERIAL() 
		{
			SAFE_RELEASE(pNormTexture);
			SAFE_RELEASE(pTexture);
		}
	};
public:
	CDX9StaticMesh( 
		HWND hWnd, 
		ID3D11Device* pDevice11,
		ID3D11DeviceContext* pContext11, 
		LPDIRECT3DDEVICE9 pDevice9,
		const char* fileName )
		: CDX9StaticMesh()
	{
		Init( hWnd, pDevice11, pContext11, pDevice9, fileName );
	}

	CDX9StaticMesh();
	~CDX9StaticMesh();

	// 初期化関数.
	HRESULT Init(
		HWND hWnd, 
		ID3D11Device* pDevice11,
		ID3D11DeviceContext* pContext11, 
		LPDIRECT3DDEVICE9 pDevice9,
		const char* fileName );

	// 描画関数.
	void Render();

private:
	// メッシュ読み込み.
	HRESULT LoadXMesh(const char* fileName);
	// 解放関数.
	void Release();
	// シェーダー作成.
	HRESULT InitShader();

	// メッシュのレンダリング.
	void RenderMesh(
		D3DXMATRIX& mWorld, 
		const D3DXMATRIX& mView, 
		const D3DXMATRIX& mProj);
	
private:
	HWND				m_hWnd;					// ウィンドウハンドル.
	LPDIRECT3DDEVICE9	m_pDevice9;				// Dx9デバイス.
	ID3D11VertexShader*	m_pVertexShader;		// 頂点シェーダー.
	ID3D11InputLayout*	m_pVertexLayout;		// 頂点レイアウト.
	ID3D11PixelShader*	m_pPixelShader;			// ピクセルシェーダー.
	ID3D11Buffer*		m_pCBufferPerMesh;		// コンスタントバッファ(メッシュ毎).
	ID3D11Buffer*		m_pCBufferPerMaterial;	// コンスタントバッファ(マテリアル毎).
	ID3D11Buffer*		m_pCBufferPerFrame;		// コンスタントバッファ(フレーム毎).

	ID3D11Buffer*		m_pVertexBuffer;		// 頂点バッファ.
	ID3D11Buffer**		m_ppIndexBuffer;		// インデックスバッファ.
	ID3D11SamplerState*	m_pSampleLinear;		// サンプラ.

	LPD3DXMESH			m_pMesh;				// メッシュオブジェクト.
	DWORD				m_NumMaterials;			// マテリアル数.

	MY_MATERIAL*	m_pMaterials;				// マテリアル構造体.
	DWORD			m_NumAttr;					// 属性数.
	DWORD			m_AttrID[300];				// 属性ID ※300属性まで.

 };

#endif	// #ifndef DX9_STATIC_MESH_H.