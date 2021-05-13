#include "Capsule.h"

CCapsule::CCapsule()
	: m_Radius	( 1.0f )
	, m_Height	( 2.0f )
{
}

CCapsule::~CCapsule()
{
}

SSegment CCapsule::GetSegment()
{
	D3DXMATRIX mRot;
	D3DXMatrixRotationYawPitchRoll( &mRot, 
		m_Tranceform.Rotation.y,
		m_Tranceform.Rotation.x,
		m_Tranceform.Rotation.z );

	// 軸ベクトルを用意.
	D3DXVECTOR3 vAxis = { 0.0, m_Height*0.5f, 0.0f };
	D3DXVECTOR3 vecAxisY;

	// Yベクトルそのものを現在の回転情報により変換する.
	D3DXVec3TransformCoord( &vecAxisY, &vAxis, &mRot );

	D3DXVECTOR3 startPos	= m_Tranceform.Position - vecAxisY;	// 始点.
	D3DXVECTOR3 endPos		= m_Tranceform.Position + vecAxisY;	// 終点.
	D3DXVECTOR3 vec			= endPos - startPos;	// ベクトル.
					
	return SSegment( startPos, vec );
}