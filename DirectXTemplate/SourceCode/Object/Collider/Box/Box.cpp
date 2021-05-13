#include "Box.h"

namespace
{
	constexpr int DIRECTION_INDEX_MAX = 3;
};

CBox::CBox()
{
}

CBox::~CBox()
{
}

//-----------------------------.
// •ûŒüƒxƒNƒgƒ‹‚ÌŽæ“¾.
//-----------------------------.
D3DXVECTOR3 CBox::GetDirection( const int& index )
{
	if( index >= DIRECTION_INDEX_MAX ) return D3DXVECTOR3();

	D3DXMATRIX mRot;
	D3DXMatrixRotationYawPitchRoll( &mRot, 
		m_Tranceform.Rotation.y,
		m_Tranceform.Rotation.x,
		m_Tranceform.Rotation.z );

	D3DXVECTOR3 dir[DIRECTION_INDEX_MAX];
	dir[0] = D3DXVECTOR3( mRot._11, mRot._12, mRot._13 );
	dir[1] = D3DXVECTOR3( mRot._21, mRot._22, mRot._23 );
	dir[2] = D3DXVECTOR3( mRot._31, mRot._32, mRot._33 );

	return dir[index];
}