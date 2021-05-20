#include "CBullet.h"
#include "..\..\..\..\Common\Mesh\Dx9StaticMesh\Dx9StaticMesh.h"
#include "..\..\..\..\Resource\MeshResource\MeshResource.h"
#include "..\..\..\..\Utility\Input\Input.h"

CBullet::CBullet()
	: m_pStaticMesh	( nullptr )

	, m_pos			()
	, m_MoveVec3	()

	, m_ShotFlag	()
{
	Init();
}

CBullet::~CBullet()
{
}

bool CBullet::Init()
{
	m_pStaticMesh = CMeshResorce::GetStatic( "Sphere" );
//	m_pStaticMesh->SetScale( D3DXVECTOR3( 0.1f, 0.1f, 0.1f ) );

	return true;
}

void CBullet::Update( const float & deltaTime )
{
	if ( m_ShotFlag == true ){

		float speed = 0.5f;
		m_pos -= m_MoveVec3 * speed;
		
		m_pStaticMesh->SetPosition( m_pos );

		if ( IsDisplayOut() == true ) m_ShotFlag = false;
	}
}

void CBullet::Render()
{
	if ( m_ShotFlag == true ){
		m_pStaticMesh->Render();
	}
}

void CBullet::Collision( CActor * pActor )
{
}

void CBullet::InitCollision()
{
}

void CBullet::UpdateCollision()
{
}

void CBullet::Shot( D3DXVECTOR3 Pos, D3DXVECTOR3 MoveVec )
{
	if ( CKeyInput::IsHold( VK_LCONTROL ) == true ) return;

	m_pos = Pos;
	m_pos.y += 4.0f;
	m_MoveVec3 = MoveVec;

	m_ShotFlag = true;
}
