#include "CollisionRender.h"
#include "..\..\..\Common\Mesh\Dx9StaticMesh\Dx9StaticMesh.h"
#include "..\..\..\Resource\MeshResource\MeshResource.h"
#include "..\..\..\Common\Mesh\RayMesh\RayMesh.h"
#include "..\Sphere\Sphere.h"
#include "..\Capsule\Capsule.h"
#include "..\Box\Box.h"
#include "..\Ray\Ray.h"
#include "..\..\..\Utility\Input\Input.h"

namespace
{
	const D3DXVECTOR4	HIT_COLOR			= { 1.0f, 0.0f, 0.0f, 1.0f };
	const D3DXVECTOR4	NOT_HIT_COLOR		= { 0.0f, 0.0f, 1.0f, 1.0f };
	constexpr char		SPHERE_MESH_NAME[]	= "Sphere";
	constexpr char		CAPSULE_MESH_NAME[]	= "Capsule";
	constexpr char		BOX_MESH_NAME[]		= "Box";
};

CCollisionRender::CCollisionRender()
	: m_pSphereQueue	()
	, m_pCapsuleQueue	()
	, m_pBoxQueue		()
	, m_pRayQueue		()
	, m_pSphere			( nullptr )
	, m_pCapsule		( nullptr )
	, m_pBox			( nullptr )
	, m_pRay			( nullptr )
	, m_IsSphreRender	( false )
	, m_IsCapsuleRender	( false )
	, m_IsBoxRender		( false )
	, m_IsRayRender		( false )
{
}

CCollisionRender::~CCollisionRender()
{
}

//--------------------------------------.
// インスタンスの取得.
//--------------------------------------.
CCollisionRender* CCollisionRender::GetInstance()
{
	static std::unique_ptr<CCollisionRender> pInstance = std::make_unique<CCollisionRender>();
	return pInstance.get();
}

//--------------------------------------.
// 描画.
//--------------------------------------.
void CCollisionRender::Render()
{
	if( CKeyInput::IsHold(VK_LCONTROL) == true &&  CKeyInput::IsMomentPress(VK_F1) == true )
		GetInstance()->m_IsSphreRender = !GetInstance()->m_IsSphreRender;
	if( CKeyInput::IsHold(VK_LCONTROL) == true &&  CKeyInput::IsMomentPress(VK_F2) == true )
		GetInstance()->m_IsCapsuleRender = !GetInstance()->m_IsCapsuleRender;
	if( CKeyInput::IsHold(VK_LCONTROL) == true &&  CKeyInput::IsMomentPress(VK_F3) == true )
		GetInstance()->m_IsBoxRender = !GetInstance()->m_IsBoxRender;
	if( CKeyInput::IsHold(VK_LCONTROL) == true &&  CKeyInput::IsMomentPress(VK_F4) == true )
		GetInstance()->m_IsRayRender = !GetInstance()->m_IsRayRender;

	GetInstance()->SphereRender();	// 球体の描画.
	GetInstance()->CapsuleRender();	// カプセルの描画.
	GetInstance()->BoxRender();		// ボックスの描画.
	GetInstance()->RayRender();		// レイの描画.
}

//--------------------------------------.
// 球体の描画.
//--------------------------------------.
void CCollisionRender::SphereRender()
{
	// モデルの取得.
	if( m_pSphere == nullptr ){
		m_pSphere = CMeshResorce::GetStatic( SPHERE_MESH_NAME );
		if( m_pSphere == nullptr ) return;
	}

	CSphere* pSphere = nullptr;
	const int queueSize = static_cast<int>(m_pSphereQueue.size());
	m_pSphere->SetRasterizerState( ERS_STATE::Wire );
	for( int i = 0; i < queueSize; i++ ){
		pSphere = m_pSphereQueue.front();

		// 衝突している状態なら色を変える.
		const D3DXVECTOR4 color = pSphere->IsHit() == true ? HIT_COLOR : NOT_HIT_COLOR;

		m_pSphere->SetColor( color );
		m_pSphere->SetPosition( pSphere->GetPosition() );
		m_pSphere->SetScale( pSphere->GetRadius() );
		m_pSphere->Render();

		m_pSphereQueue.pop();
	}
	m_pSphere->SetRasterizerState( ERS_STATE::None );
}

//--------------------------------------.
// カプセルの描画.
//--------------------------------------.
void CCollisionRender::CapsuleRender()
{
	// モデルの取得.
	if( m_pCapsule == nullptr ){
		m_pCapsule = CMeshResorce::GetStatic( CAPSULE_MESH_NAME );
		if( m_pCapsule == nullptr ) return;
	}

	CCapsule* pCapsule = nullptr;
	const int queueSize = static_cast<int>(m_pCapsuleQueue.size());
	m_pCapsule->SetRasterizerState( ERS_STATE::Wire );
	for( int i = 0; i < queueSize; i++ ){
		pCapsule = m_pCapsuleQueue.front();

		// 衝突している状態なら色を変える.
		const D3DXVECTOR4 color = pCapsule->IsHit() == true ? HIT_COLOR : NOT_HIT_COLOR;

		m_pCapsule->SetColor( color );
		m_pCapsule->SetPosition( pCapsule->GetPosition() );
		m_pCapsule->SetRotation( pCapsule->GetRotation() );
		const D3DXVECTOR3 scale = 
		{
			pCapsule->GetRadius(),
			pCapsule->GetHeight()*0.5f,
			pCapsule->GetRadius(),
		};
		m_pCapsule->SetScale( scale );
		m_pCapsule->Render();

		m_pCapsuleQueue.pop();
	}
	m_pCapsule->SetRasterizerState( ERS_STATE::None );
}

//--------------------------------------.
// ボックスの描画.
//--------------------------------------.
void CCollisionRender::BoxRender()
{
	// モデルの取得.
	if( m_pBox == nullptr ){
		m_pBox = CMeshResorce::GetStatic( BOX_MESH_NAME );
		if( m_pBox == nullptr ) return;
	}

	CBox* pBox = nullptr;
	const int queueSize = static_cast<int>(m_pBoxQueue.size());
	m_pBox->SetRasterizerState( ERS_STATE::Wire );
	for( int i = 0; i < queueSize; i++ ){
		pBox = m_pBoxQueue.front();

		// 衝突している状態なら色を変える.
		const D3DXVECTOR4 color = pBox->IsHit() == true ? HIT_COLOR : NOT_HIT_COLOR;

		m_pBox->SetColor( color );
		m_pBox->SetPosition( pBox->GetPosition() );
		m_pBox->SetRotation( pBox->GetRotation() );
		m_pBox->SetScale( pBox->GetScale() );
		m_pBox->Render();

		m_pBoxQueue.pop();
	}
	m_pBox->SetRasterizerState( ERS_STATE::None );
}

// レイの描画.
void CCollisionRender::RayRender()
{
	// モデルの取得.
	if( m_pRay == nullptr ){
		m_pRay = std::make_unique<CRayMesh>();
		if( FAILED( m_pRay->Init( CDirectX11::GetContext() ) )) return;
	}

	CRay* pRay = nullptr;
	const int queueSize = static_cast<int>(m_pRayQueue.size());
	for( int i = 0; i < queueSize; i++ ){
		pRay = m_pRayQueue.front();

		// 衝突している状態なら色を変える.
		const D3DXVECTOR4 color = pRay->IsHit() == true ? HIT_COLOR : NOT_HIT_COLOR;

		m_pRay->SetColor( color );
		m_pRay->SetPosition( pRay->GetStartPos() );
		m_pRay->SetRotation( pRay->GetRotation() );
		m_pRay->SetScale( pRay->GetScale() );
		m_pRay->SetVector( pRay->GetVector() );
		m_pRay->Render();

		m_pRayQueue.pop();
	}
}

//--------------------------------------.
// 球体の追加.
//--------------------------------------.
void CCollisionRender::PushSphere( CSphere* pSphere )
{
	if( GetInstance()->m_IsSphreRender == false ) return;
	GetInstance()->m_pSphereQueue.push( pSphere );
}

//--------------------------------------.
// カプセルの追加.
//--------------------------------------.
void CCollisionRender::PushCapsule( CCapsule* pCapsule )
{
	if( GetInstance()->m_IsCapsuleRender == false ) return;
	GetInstance()->m_pCapsuleQueue.push( pCapsule );
}

//--------------------------------------.
// ボックスの追加.
//--------------------------------------.
void CCollisionRender::PushBox( CBox* pBox )
{
	if( GetInstance()->m_IsBoxRender == false ) return;
	GetInstance()->m_pBoxQueue.push( pBox );
}

//--------------------------------------.
// レイの追加.
//--------------------------------------.
void CCollisionRender::PushRay( CRay* pBox )
{
	if( GetInstance()->m_IsRayRender == false ) return;
	GetInstance()->m_pRayQueue.push( pBox );
}
