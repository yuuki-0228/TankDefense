#include "Math.h"

//-------------------------------------------------------------.
// vのベクトルに対して直交のベクトルを返す.
//-------------------------------------------------------------.
D3DXVECTOR3 Math::Orthogonal( const D3DXVECTOR3& v )
{
	const float x = fabsf( v.x );
	const float y = fabsf( v.y );
	const float z = fabsf( v.z );

	D3DXVECTOR3 vOther = x < y ? (x < z ? X_AXIS : Z_AXIS) : (y < z ? Y_AXIS : Z_AXIS);

	D3DXVECTOR3 vResult;
	D3DXVec3Cross( &vResult, &v, &vOther );

	return vResult;
}

//-------------------------------------------------------------.
// uの方向をvの方向と同じにするためのD3DXQUATERNIONを返す.
//-------------------------------------------------------------.
D3DXQUATERNION Math::GetRotationBetween( const D3DXVECTOR3& _u, const D3DXVECTOR3& _v )
{
	D3DXQUATERNION q;

	D3DXVECTOR3 u = _u, v = _v;
	// ベクトルを正規化.
	D3DXVec3Normalize( &u, &u );
	D3DXVec3Normalize( &v, &v );

	/*
	* We have to check for when u == -v, as u + v
	* in this case will be (0, 0, 0), which cannot be normalized.
	*
	* We account for floating point inaccuracies by taking the dot product (-1.0 when u == -v)
	* and checking if it is close enough to -1.0f

	* ﾊｰﾌﾍﾞｸﾄﾙ（u + v）を正規化して計算しているため
	* 方向が真逆の場合は (u == -v) -> u + v == (0, 0, 0)
	* (0, 0, 0) は正規化できないため、それを考慮しないといけない。
	*
	* 真逆の場合は、uに対して直交の軸で180度回転で済むので、計算がそれほど難しくない.
	* しかし、浮動小数点相対精度を考慮しないといけないので、DOT演算で真逆かどうかの判断をする.
	*
	* u == -v の時、ドット演算が-1.0になる.
	*/
	const float UoV = D3DXVec3Dot( &u, &v );
	if( UoV <= -1.0f + FLT_EPSILON ){ // FLT_EPSILONで浮動小数点相対精度を考慮.
		// エッジケース.

		// 直交の軸で180度回転.
		D3DXVECTOR3 vOrthogonal = Orthogonal( u );
		D3DXVec3Normalize( &vOrthogonal, &vOrthogonal );

		q.x = vOrthogonal.x;
		q.y = vOrthogonal.y;
		q.z = vOrthogonal.z;
		q.w = 0.0f;
	} else {
		// 通常ｹｰｽ

		D3DXVECTOR3 vHalf;
		D3DXVec3Normalize( &vHalf, &(u + v) );

		D3DXVECTOR3 vCrossUHalf;
		D3DXVec3Cross( &vCrossUHalf, &u, &vHalf );

		q.x = vCrossUHalf.x;
		q.y = vCrossUHalf.y;
		q.z = vCrossUHalf.z;
		q.w = D3DXVec3Dot( &u, &vHalf );
	}

	return q;
}
