#include "NormalLight.h"
#include "..\..\..\Utility\Input\Input.h"

CNormalLight::CNormalLight()
{
}

CNormalLight::~CNormalLight()
{
}

// çXêVä÷êî.
void CNormalLight::Update( const float& deltaTime )
{
	m_Direction = m_Tranceform.Position - m_LookPosition;

	if( CKeyInput::IsHold('L') == true && CKeyInput::IsMomentPress('M') == true ) m_IsActive = !m_IsActive;
	if( m_IsActive == false ) return;

	SimpleMove( deltaTime );
}