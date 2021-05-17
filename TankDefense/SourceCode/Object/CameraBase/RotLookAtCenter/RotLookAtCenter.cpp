#include "RotLookAtCenter.h"
#include "..\..\..\Utility\Input\Input.h"

CRotLookAtCenter::CRotLookAtCenter()
	: m_vDegree			{ 0.0f, 0.0f }
	, m_Length			( DEFAULT_LENGTH )
	, m_HorizontalMax	( DEFAULT_HORIZONTAL_MAX )
	, m_HorizontalMin	( DEFAULT_HORIZONTAL_MIN )
	, m_VerticalMax		( DEFAULT_VERTICAL_MAX )
	, m_VerticalMin		( DEFAULT_VERTICAL_MIN )
	, m_RightMoveSpeed	( DEFAULT_MOVE_SPEED )
	, m_LeftMoveSpeed	(-DEFAULT_MOVE_SPEED )
	, m_AddValue		( 0.0f, 0.0f, 0.0f )
{
}

CRotLookAtCenter::CRotLookAtCenter( const bool& isLoad )
	: CRotLookAtCenter	()
{
}

CRotLookAtCenter::~CRotLookAtCenter()
{
}

// パラメーターの読み込み.
bool CRotLookAtCenter::LoadParameter( const bool& isLoad )
{
	return true;
}

// 更新.
void CRotLookAtCenter::Update( const float& deltaTime )
{
	if ( CKeyInput::IsHold( VK_LCONTROL ) == false ) return;

	// 前進.
	if ( CKeyInput::IsHold( 'W' ) == true || CXInput::GetLeftThumbY() > IDLE_THUMB_MAX )
		m_Tranceform.Position.z += 0.1f;
	// 後退.
	if ( CKeyInput::IsHold( 'S' ) == true || CXInput::GetLeftThumbY() < IDLE_THUMB_MIN )
		m_Tranceform.Position.z -= 0.1f;
	// 右に移動.
	if ( CKeyInput::IsHold( 'D' ) == true || CXInput::GetLeftThumbX() > IDLE_THUMB_MAX )
		m_Tranceform.Position.x += 0.1f;
	// 左に移動.
	if ( CKeyInput::IsHold( 'A' ) == true || CXInput::GetLeftThumbX() < IDLE_THUMB_MIN )
		m_Tranceform.Position.x -= 0.1f;
	// 上昇.
	if ( CKeyInput::IsHold( 'E' ) == true || CXInput::GetRightTrigger() > IDLE_TIGGER_MAX )
		m_Tranceform.Position.y += 0.1f;
	// 下降.
	if ( CKeyInput::IsHold( 'Q' ) == true || CXInput::GetLeftTrigger() > IDLE_TIGGER_MAX )
		m_Tranceform.Position.y -= 0.1f;

//	m_Tranceform.Position = { 10.0f, 10.0f, 10.0f };
}

//-------------------------------.
// オブジェクトをを注視に回転.
//-------------------------------.
void CRotLookAtCenter::RotationLookAtObject( const D3DXVECTOR3& vLookPos, const float& attenRate )
{
	// 注視位置を取得.
	m_LookPosition = vLookPos;

	D3DXVECTOR3 pos;	// 本来の座標.
	// カメラ位置を算出.
	pos.x = m_LookPosition.x + ( sinf(m_vDegree.x) * m_Length );
	pos.y = m_Tranceform.Position.y;
	pos.z = m_LookPosition.z + ( cosf(m_vDegree.x) * m_Length );
	// lerp減衰. ( out, 現在の座標, 本来到達している座標, 補正値 ).
	D3DXVec3Lerp( &m_Tranceform.Position, &m_Tranceform.Position, &pos, attenRate );
	m_Tranceform.Position += m_AddValue;
	m_AddValue = { 0.0f, 0.0f, 0.0f };
}

//-------------------------------.
// 横方向の回転.
//-------------------------------.
void CRotLookAtCenter::DegreeHorizontalMove( const float& movePower )
{
	// 横方向に足し合わせる.
	m_vDegree.x += movePower;
}

//-------------------------------.
// 横方向の右回転.
//-------------------------------.
void CRotLookAtCenter::DegreeHorizontalRightMove()
{
	DegreeHorizontalMove( m_RightMoveSpeed );
}

//-------------------------------.
// 横方向の左回転.
//-------------------------------.
void CRotLookAtCenter::DegreeHorizontalLeftMove()
{
	DegreeHorizontalMove( m_LeftMoveSpeed );
}

//-------------------------------.
// 縦方向の回転.
//-------------------------------.
void CRotLookAtCenter::DegreeVerticalMove( const float& movePower )
{
	// 縦方向に足し合わせる.
	m_vDegree.y += movePower;
}

//-------------------------------.
// カメラと注視点の長さ設定.
//-------------------------------.
void CRotLookAtCenter::SetLength( const float& length )
{
	m_Length = length;
}

//-------------------------------.
// 高さの設定.
//-------------------------------.
void CRotLookAtCenter::SetHeight( const float& height )
{
	m_Tranceform.Position.y = height;
}

//-------------------------------.
// 横方向の最大、最小の設定.
//-------------------------------.
void CRotLookAtCenter::SetHorizontalDegree( const float& max, const float& min )
{
	m_HorizontalMax	= max;
	m_HorizontalMin	= min;
}

//-------------------------------.
// 縦方向の最大、最小の設定.
//-------------------------------.
void CRotLookAtCenter::SetVerticalDegree( const float& max, const float& min )
{	
	m_VerticalMax = max;
	m_VerticalMin = min;
}

//-------------------------------.
// 座標に足し合わせる値の設定.
//-------------------------------.
void CRotLookAtCenter::SetAddPositionValue( const D3DXVECTOR3& addValue )
{
	m_AddValue = addValue;
}