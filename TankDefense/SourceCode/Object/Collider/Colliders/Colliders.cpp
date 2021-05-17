#include "Colliders.h"

CCollisions::CCollisions()
	: m_pSphere		( nullptr )
	, m_pCapsule	( nullptr )
	, m_pBox		( nullptr )
	, m_pRay		( nullptr )
{
}

CCollisions::~CCollisions()
{
}

// éwíËÇµÇΩìñÇΩÇËîªíËÇÃèâä˙âª.
void CCollisions::InitCollision( const ECollNo& no, const STranceform& t )
{
	switch( no )
	{
	case ECollNo::Sphere:
		m_pSphere = std::make_unique<CSphere>();
		m_pSphere->SetTranceform( t );
	case ECollNo::Capsule:
		m_pCapsule = std::make_unique<CCapsule>();
		m_pCapsule->SetTranceform( t );
	case ECollNo::Box:
		m_pBox = std::make_unique<CBox>();
		m_pBox->SetTranceform( t );
	case ECollNo::Ray:
		m_pRay = std::make_unique<CRay>();
		m_pRay->SetTranceform( t );
		break;
	case ECollNo::Mesh:
		m_pMesh = std::make_unique<CMesh>();
		m_pMesh->SetTranceform( t );
	default:
		break;
	}
}