#ifndef TRANCEFORM_H
#define TRANCEFORM_H

// 警告についてのｺｰﾄﾞ分析を無効にする.4005:再定義.
#pragma warning(disable:4005)

#include <D3DX11.h>
#include <D3DX10.h>	//「D3DX～」の定義使用時に必要.
#include <D3D10.h>

// ライブラリ読み込み.
#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "d3dx11.lib" )
#pragma comment( lib, "d3dx10.lib" )	//「D3DX～」の定義使用時に必要.

struct stTranceform
{
	D3DXVECTOR3 Position;	// 座標.
	D3DXVECTOR3 Rotation;	// 回転.
	D3DXVECTOR3 Scale;		// 拡大縮小.

	stTranceform()
		: Position	( 0.0f, 0.0f, 0.0f )
		, Rotation	( 0.0f, 0.0f, 0.0f )
		, Scale		( 1.0f, 1.0f, 1.0f )
	{}

	stTranceform( 
		const D3DXVECTOR3& pos,
		const D3DXVECTOR3& rot,
		const D3DXVECTOR3& scale )
		: Position	( pos )
		, Rotation	( rot )
		, Scale		( scale )
	{}

	//　ワールド行列, スケール行列, 回転行列, 平行移動行列.
	D3DXMATRIX GetWorldMatrix() const
	{
		D3DXMATRIX mScale, mRot, mTran;

		// 拡大縮小行列作成.
		D3DXMatrixScaling( &mScale, Scale.x, Scale.y, Scale.z );
		// 回転行列を作成.
		D3DXMatrixRotationYawPitchRoll( &mRot, Rotation.y, Rotation.x, Rotation.z );
		// 平行移動行列.
		D3DXMatrixTranslation( &mTran, Position.x, Position.y, Position.z );

		// ワールド行列作成.
		return mScale * mRot * mTran;
	}

} typedef STranceform;

#endif	// #ifndef TRANCEFORM_H.