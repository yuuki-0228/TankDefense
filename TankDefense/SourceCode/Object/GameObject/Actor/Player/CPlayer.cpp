#include "CPlayer.h"
#include "..\..\..\..\Common\Mesh\RayMesh\RayMesh.h"
#include "..\..\..\..\Common\Mesh\Dx9SkinMesh\Dx9SkinMesh.h"
#include "..\..\..\..\Resource\MeshResource\MeshResource.h"
#include "..\..\..\..\Utility\Input\Input.h"
#include "..\..\..\..\Object\CameraBase\CameraManager\CameraManager.h"
#include "..\..\..\..\Object\CameraBase\RotLookAtCenter\RotLookAtCenter.h"
#include "..\..\..\..\Object\GameObject\Actor\Bullet\CBullet.h"

CPlayer::CPlayer()
	: m_pSkinMesh	( nullptr )
	, m_pLookCamera	( nullptr )

	, m_pos			()
	, m_Rot			()
	, m_MoveVec3	()

	, m_pBullet		()
{
	Init();
}

CPlayer::~CPlayer()
{
	SAFE_DELETE( m_pBullet );
}

bool CPlayer::Init()
{
	m_pLookCamera = std::make_unique<CRotLookAtCenter>();
	m_pLookCamera->Update( 0.0f );	// 視点座標を更新しておく.

	m_pSkinMesh = CMeshResorce::GetSkin( "a_s" );
	m_pSkinMesh->SetAnimSpeed( GetDeltaTime<double>() );

	m_pBullet = new CBullet();

	return true;
}

void CPlayer::Update( const float & deltaTime )
{
	const float speed = 0.1f;

	if ( CKeyInput::IsPress( VK_LEFT	) == true ) m_pLookCamera->DegreeHorizontalLeftMove();
	if ( CKeyInput::IsPress( VK_RIGHT	) == true ) m_pLookCamera->DegreeHorizontalRightMove();
	if ( CKeyInput::IsPress( VK_UP		) == true ) m_pos -= m_MoveVec3 * speed;
	if ( CKeyInput::IsPress( VK_DOWN	) == true ) m_pos += m_MoveVec3 * speed;
	if ( CKeyInput::IsPress( VK_SPACE ) == true ){

		m_pBullet->Shot( m_pos, m_MoveVec3 );
	}

	//カメラの横回転に応じて回転させる.
	m_Rot.y = m_pLookCamera->GetRadianX();

	//モデルを移動、回転させる.
	m_pSkinMesh->SetPosition( m_pos );
	m_pSkinMesh->SetRotation( m_Rot );

	//移動ベクトルの設定.
	m_MoveVec3.x = sinf( m_Rot.y );
	m_MoveVec3.z = cosf( m_Rot.y );

	//カメラを設定.
	m_pLookCamera->RotationLookAtObject( m_pos, speed );
	CCameraManager::ChangeCamera( m_pLookCamera.get() );

	m_pBullet->Update( m_DeltaTime );
}

void CPlayer::Render()
{
	m_pSkinMesh->Render();

	m_pBullet->Render();
}

inline void CPlayer::Collision( CActor * pActor )
{
}

void CPlayer::InitCollision()
{
}

void CPlayer::UpdateCollision()
{
}
