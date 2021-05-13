#ifndef MTAH_H
#define MTAH_H

// 警告についてのｺｰﾄﾞ分析を無効にする.4005:再定義.
#pragma warning(disable:4005)

#include <D3DX11.h>
#include <D3DX10.h>	//「D3DX～」の定義使用時に必要.
#include <D3D10.h>

// ライブラリ読み込み.
#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "d3dx11.lib" )
#pragma comment( lib, "d3dx10.lib" )	//「D3DX～」の定義使用時に必要.

namespace Math
{
	const D3DXVECTOR3 X_AXIS{ 1.0f, 0.0f, 0.0f };
	const D3DXVECTOR3 Y_AXIS{ 0.0f, 1.0f, 0.0f };
	const D3DXVECTOR3 Z_AXIS{ 0.0f, 0.0f, 1.0f };

	// vのベクトルに対して直交のベクトルを返す.
	extern D3DXVECTOR3 Orthogonal( const D3DXVECTOR3& v );

	// uの方向をvの方向と同じにするためのD3DXQUATERNIONを返す.
	extern D3DXQUATERNION GetRotationBetween( const D3DXVECTOR3& _u, const D3DXVECTOR3& _v );
};

#endif	// #ifndef MTAH_H.