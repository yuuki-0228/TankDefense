#ifndef MESH_STRUCT_H
#define MESH_STRUCT_H

#include "..\Common.h"

// メッシュ単位で渡す情報.
struct CBUFFER_PER_MESH
{
	D3DXMATRIX	mW;		// ワールド行列.
	D3DXMATRIX	mWVP;	// ワールド,ビュー,プロジェクションの合成変換行列.
	D3DXVECTOR4 vColor;	// 色.
};

// マテリアル単位で渡す情報.
struct CBUFFER_PER_MATERIAL
{
	D3DXVECTOR4 vAmbient;	// アンビエント色(環境色).
	D3DXVECTOR4	vDiffuse;	// ディフューズ色(拡散反射色).
	D3DXVECTOR4	vSpecular;	// スペキュラ色(鏡面反射色).
};

// フレーム単位で渡す情報.
struct CBUFFER_PER_FRAME
{
	D3DXVECTOR4	vCamPos;		// カメラ位置(視点位置).
	D3DXVECTOR4	vLightDir;		// ライト方向.
	D3DXVECTOR4 vIntensity;		// ライト強度(明るさ : xのみ使用).
};

#endif	// #ifndef MESH_STRUCT_H.