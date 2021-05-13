#include "LightManager.h"
#include "..\NormalLight\NormalLight.h"
#include "..\..\..\Common\Mesh\Dx9StaticMesh\Dx9StaticMesh.h"
#include "..\..\..\Resource\MeshResource\MeshResource.h"

namespace
{
	const D3DXVECTOR4	DEBUG_MESH_COLOR = { 1.0f, 1.0f, 1.0f, 1.0f };	// メッシュの色.
	constexpr float		DEBUG_MESH_SCALE = 0.5f;						// メッシュのサイズ.
};

CLightManager::CLightManager()
	: m_pLight			( nullptr )
	, m_pNormalLight	( nullptr )
	, m_LightStateQueue	()
#ifdef _DEBUG
	, m_StaticMesh		( nullptr )
#endif // #ifdef _DEBUG.
{
	m_pNormalLight = std::make_unique<CNormalLight>();
	m_pLight = m_pNormalLight.get();
}

CLightManager::~CLightManager()
{
}

//--------------------------------------.
// インスタンスの取得.
//--------------------------------------.
CLightManager* CLightManager::GetInstance()
{
	static std::unique_ptr<CLightManager> pInstance = std::make_unique<CLightManager>();
	return pInstance.get();
}

//--------------------------------------.
// 更新関数.
//--------------------------------------.
void CLightManager::Update( const float& deltaTime )
{
	if( GetInstance()->m_pLight == nullptr ) return;

	GetInstance()->m_pLight->Update( deltaTime );

	const D3DXVECTOR3 pos		= GetInstance()->m_pLight->GetPosition();
	const D3DXVECTOR3 lookPos	= GetInstance()->m_pLight->GetLookPosition();
	const D3DXVECTOR3 direction	= GetInstance()->m_pLight->GetDirection();
	const D3DXVECTOR3 color		= GetInstance()->m_pLight->GetColor();

	CDebugText::PushText( "-----------------" );
	CDebugText::PushText( "----  Light  ----" );
	CDebugText::PushText( "-----------------" );
	CDebugText::PushText( "'L + M' Is Light Activce Change" );
	CDebugText::PushText( "'P' Is Position, 'L' Is LookPosition + " );
	CDebugText::PushText( "'D or A' X Move, 'E or Q' Y Move, 'W or S' Z Move" );
	CDebugText::PushText( "-----------------" );
	CDebugText::PushText( "IsActive  : ", GetInstance()->m_pLight->IsActive() == true ? "true" : "false" );
	CDebugText::PushText( "Pos       : ", pos.x,		", ", pos.y,		", ", pos.z );
	CDebugText::PushText( "LookPos   : ", lookPos.x,	", ", lookPos.y,	", ", lookPos.z );
	CDebugText::PushText( "Direction : ", direction.x,	", ", direction.y,	", ", direction.z );
	CDebugText::PushText( "Color     : ", color.x,		", ", color.y,		", ", color.z );
}

// 位置の描画.
void CLightManager::PositionRender()
{
#ifdef _DEBUG

	if( GetInstance()->m_pLight->IsActive() == false ) return;

	if( GetInstance()->m_StaticMesh == nullptr ){
		GetInstance()->m_StaticMesh = CMeshResorce::GetStatic("Sphere");
		if( GetInstance()->m_StaticMesh == nullptr ) return;
	}

	GetInstance()->m_StaticMesh->SetScale( DEBUG_MESH_SCALE );
	GetInstance()->m_StaticMesh->SetColor( DEBUG_MESH_COLOR );
	GetInstance()->m_StaticMesh->SetPosition( GetInstance()->m_pLight->GetPosition() );
	GetInstance()->m_StaticMesh->Render();
	GetInstance()->m_StaticMesh->SetPosition( GetInstance()->m_pLight->GetLookPosition() );
	GetInstance()->m_StaticMesh->Render();

#endif // #ifdef _DEBUG.
}