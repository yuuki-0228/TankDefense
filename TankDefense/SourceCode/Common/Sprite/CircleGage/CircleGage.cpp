#include "CircleGage.h"
#include "..\..\Shader\Shader.h"
#include "..\..\..\Object\CameraBase\CameraManager\CameraManager.h"

#include <algorithm>

namespace
{
	const char* SHADER_NAME = "Data\\Shader\\Sprite\\GageSprite.hlsl";
	const char* PS_SHADER_ENTRY_NAMES[] =
	{
		"PS_CircleMain",
	};
};

CCircleGageSprite::CCircleGageSprite()
	: m_CenterPosition	( 0.0f, 0.0f )
	, m_StartVector		( 0.0f, 1.0f )
	, m_Value			( 0.5f )
	, m_Angle			( static_cast<float>(D3DX_PI)*2.0f )
	, m_IsClockwise		( true )
{
}

CCircleGageSprite::~CCircleGageSprite()
{
	CSprite::Release();
}

//---------------------------------------.
// ������.
//---------------------------------------.
HRESULT CCircleGageSprite::Init( ID3D11DeviceContext* pContext11, const char* texturePath )
{
	if( FAILED( SetDevice( CDirectX11::GetContext() ) ))	return E_FAIL;
	if( FAILED( VertexInitShader( SHADER_NAME ) ))			return E_FAIL;
	if( FAILED( CCircleGageSprite::PixelInitShader() ))		return E_FAIL;
	if( FAILED( InitSample() ))								return E_FAIL;
	if( FAILED( CCircleGageSprite::InitConstantBuffer() ))	return E_FAIL;

	return S_OK;
}

//---------------------------------------.
// ���.
//---------------------------------------.
void CCircleGageSprite::Release()
{
	CSprite::Release();
}

//---------------------------------------.
// �`��.
//---------------------------------------.
void CCircleGageSprite::Render( SSpriteRenderState* pRenderState )
{
	if( m_pSrcTexture == nullptr ) return;
	if( m_pVertexShaderUI == nullptr ) return;

	SSpriteRenderState* renderState = pRenderState == nullptr ? &m_SpriteRenderState : pRenderState;
	SSpriteAnimState* animState = &renderState->AnimState;

	animState->AnimUpdate();		// �A�j���[�V�����̍X�V.
	animState->UVScrollUpdate();	// �X�N���[���̍X�V.

	//�@���[���h�s��.
	D3DXMATRIX mWorld = renderState->Tranceform.GetWorldMatrix();

	//�@���[���h�s��, �X�P�[���s��, ��]�s��, ���s�ړ��s��.
	D3DXMATRIX mWVP;

	// �V�F�[�_�[�̃R���X�^���g�o�b�t�@�Ɋe��f�[�^��n��.
	D3D11_MAPPED_SUBRESOURCE pData;
	CBUFFER_PER_FRAME cb;	// �R���X�^���g�o�b�t�@.

	// �o�b�t�@���̃f�[�^�̏��������J�n����Map.
	if( SUCCEEDED( m_pContext11->Map(
		m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData ))){

		cb.mWVP	= mWorld * CCameraManager::GetViewProjMatrix();
		cb.mW	= mWorld;
		D3DXMatrixTranspose( &cb.mWVP, &cb.mWVP );
		D3DXMatrixTranspose( &cb.mW, &cb.mW );

		// �r���[�|�[�g�̕�,������n��.
		cb.vViewPort.x	= static_cast<float>(WND_W);
		cb.vViewPort.y	= static_cast<float>(WND_H);

		// �F��n��.
		cb.vColor = renderState->Color;
		// �e�N�X�`�����W.
		cb.vUV = animState->UV;

		// �~�̒��S���W.
		// 0~1�܂łȂ̂ŁA0.5�����S.
		cb.vCenterPos = m_CenterPosition;

		// �~�̊J�n�x�N�g��.
		// 2D�Ȃ̂ŁA��������}�C�i�X.
		cb.vStartVector = m_StartVector;

		// �l 0~1 ��radian�Ƃ��ēn��.
		cb.Value	= m_Angle*std::clamp(m_Value, 0.0f, 1.0f);

		// ���v���ɂ��邩�ǂ����ݒ�.
		cb.IsClockwise = m_IsClockwise == true ? 1.0f : 0.0f;

		memcpy_s( pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb) );

		m_pContext11->Unmap( m_pConstantBuffer, 0 );
	}

	// �g�p����V�F�[�_�̃Z�b�g.
	m_pContext11->VSSetShader( m_pVertexShaderUI, nullptr, 0 );	// ���_�V�F�[�_.
	m_pContext11->PSSetShader( m_pPixelShaders[EGageNo_Circle], nullptr, 0 );	// �s�N�Z���V�F�[�_.

	// ���̃R���X�^���g�o�b�t�@���ǂ̃V�F�[�_�Ŏg�p���邩�H.
	m_pContext11->VSSetConstantBuffers( 0, 1, &m_pConstantBuffer );	// ���_�V�F�[�_.
	m_pContext11->PSSetConstantBuffers( 0, 1, &m_pConstantBuffer );	// �s�N�Z���V�F�[�_�[.

	// ���_�o�b�t�@���Z�b�g.
	UINT stride = sizeof(SpriteVertex); // �f�[�^�̊Ԋu.
	UINT offset = 0;
	m_pContext11->IASetVertexBuffers( 0, 1, &m_pVertexBufferUI, &stride, &offset );

	// ���_�C���v�b�g���C�A�E�g���Z�b�g.
	m_pContext11->IASetInputLayout( m_pVertexLayout );

	// �v���~�e�B�u�E�g�|���W�[���Z�b�g.
	m_pContext11->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

	// �e�N�X�`�����V�F�[�_�[�ɓn��.
	m_pContext11->PSSetSamplers( 0, 1, &m_pSampleLinears[renderState->SmaplerNo] );
	m_pContext11->PSSetShaderResources( 0, 1, &m_pSrcTexture );
	m_pContext11->PSSetShaderResources( 1, 1, &m_pDestTexture );
	m_pContext11->PSSetShaderResources( 2, 1, &m_pMaskTexture );
	m_pContext11->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

	// �����_�����O.
	SetBlend( true );
	SetDeprh( false );
	SetRasterizerState( ERS_STATE::Back );
	m_pContext11->Draw( 4, 0 );
	SetRasterizerState( ERS_STATE::None );
	SetDeprh( true );
	SetBlend( false );
	m_pDestTexture = nullptr;
	m_pMaskTexture = nullptr;

	ID3D11ShaderResourceView* resetSrv = nullptr;
	m_pContext11->PSSetShaderResources( 0, 1, &resetSrv );
	m_pContext11->PSSetShaderResources( 1, 1, &resetSrv );
	m_pContext11->PSSetShaderResources( 2, 1, &resetSrv );
}

//---------------------------------------.
// 3D�`��.
//---------------------------------------.
void CCircleGageSprite::Render3D( const bool& isBillboard, SSpriteRenderState* pRenderState )
{
	if( m_pSrcTexture == nullptr ) return;
	if( m_pVertexBuffer3D == nullptr ) return;

	SSpriteRenderState* renderState = pRenderState == nullptr ? &m_SpriteRenderState : pRenderState;
	SSpriteAnimState* animState = &renderState->AnimState;

	animState->AnimUpdate();		// �A�j���[�V�����̍X�V.
	animState->UVScrollUpdate();	// �X�N���[���̍X�V.

	//�@���[���h�s��.
	D3DXMATRIX mWorld = renderState->Tranceform.GetWorldMatrix();

	if( isBillboard == true ){
		// �r���{�[�h�p.
		D3DXMATRIX CancelRotation = CCameraManager::GetViewMatrix();
		CancelRotation._41 = CancelRotation._42 = CancelRotation._43 = 0.0f; // xyz��0�ɂ���.
		 // CancelRotation�̋t�s������߂�.
		D3DXMatrixInverse( &CancelRotation, nullptr, &CancelRotation );
		mWorld = CancelRotation * mWorld;
	}

	//�@���[���h�s��, �X�P�[���s��, ��]�s��, ���s�ړ��s��.
	D3DXMATRIX mWVP;

	// �V�F�[�_�[�̃R���X�^���g�o�b�t�@�Ɋe��f�[�^��n��.
	D3D11_MAPPED_SUBRESOURCE pData;
	CBUFFER_PER_FRAME cb;	// �R���X�^���g�o�b�t�@.

	// �o�b�t�@���̃f�[�^�̏��������J�n����Map.
	if( SUCCEEDED( m_pContext11->Map(
		m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData ))){

		cb.mWVP	= mWorld * CCameraManager::GetViewProjMatrix();
		cb.mW	= mWorld;
		D3DXMatrixTranspose( &cb.mWVP, &cb.mWVP );
		D3DXMatrixTranspose( &cb.mW, &cb.mW );

		// �r���[�|�[�g�̕�,������n��.
		cb.vViewPort.x	= static_cast<float>(WND_W);
		cb.vViewPort.y	= static_cast<float>(WND_H);

		// �F��n��.
		cb.vColor = renderState->Color;
		// �e�N�X�`�����W.
		cb.vUV = animState->UV;

		// �~�̒��S���W.
		// 0~1�܂łȂ̂ŁA0.5�����S.
		cb.vCenterPos = m_CenterPosition;

		// �~�̊J�n�x�N�g��.
		// 2D�Ȃ̂ŁA��������}�C�i�X.
		cb.vStartVector = m_StartVector;

		// �l 0~1 ��radian�Ƃ��ēn��.
		cb.Value	= m_Angle*std::clamp(m_Value, 0.0f, 1.0f);

		// ���v���ɂ��邩�ǂ����ݒ�.
		cb.IsClockwise = m_IsClockwise == true ? 1.0f : 0.0f;

		memcpy_s( pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb) );

		m_pContext11->Unmap( m_pConstantBuffer, 0 );
	}

	// �g�p����V�F�[�_�̃Z�b�g.
	m_pContext11->VSSetShader( m_pVertexShader3D, nullptr, 0 );	// ���_�V�F�[�_.
	m_pContext11->PSSetShader( m_pPixelShaders[EGageNo_Circle], nullptr, 0 );	// �s�N�Z���V�F�[�_.

	// ���̃R���X�^���g�o�b�t�@���ǂ̃V�F�[�_�Ŏg�p���邩�H.
	m_pContext11->VSSetConstantBuffers( 0, 1, &m_pConstantBuffer );	// ���_�V�F�[�_.
	m_pContext11->PSSetConstantBuffers( 0, 1, &m_pConstantBuffer );	// �s�N�Z���V�F�[�_�[.

	// ���_�o�b�t�@���Z�b�g.
	UINT stride = sizeof(SpriteVertex); // �f�[�^�̊Ԋu.
	UINT offset = 0;
	m_pContext11->IASetVertexBuffers( 0, 1, &m_pVertexBuffer3D, &stride, &offset );

	// ���_�C���v�b�g���C�A�E�g���Z�b�g.
	m_pContext11->IASetInputLayout( m_pVertexLayout );

	// �v���~�e�B�u�E�g�|���W�[���Z�b�g.
	m_pContext11->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

	// �e�N�X�`�����V�F�[�_�[�ɓn��.
	m_pContext11->PSSetSamplers( 0, 1, &m_pSampleLinears[renderState->SmaplerNo] );
	m_pContext11->PSSetShaderResources( 0, 1, &m_pSrcTexture );
	m_pContext11->PSSetShaderResources( 1, 1, &m_pDestTexture );
	m_pContext11->PSSetShaderResources( 2, 1, &m_pMaskTexture );
	m_pContext11->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

	// �`��.
	SetBlend( true );
	m_pContext11->Draw( 4, 0 );
	SetBlend( false );
	m_pDestTexture = nullptr;
	m_pMaskTexture = nullptr;

	ID3D11ShaderResourceView* resetSrv = nullptr;
	m_pContext11->PSSetShaderResources( 0, 1, &resetSrv );
	m_pContext11->PSSetShaderResources( 1, 1, &resetSrv );
	m_pContext11->PSSetShaderResources( 2, 1, &resetSrv );
}

//---------------------------------------.
// �s�N�Z���V�F�[�_�[�̍쐬.
//---------------------------------------.
HRESULT CCircleGageSprite::PixelInitShader()
{
	ID3DBlob* pCompiledShader = nullptr;
	ID3DBlob* pErrors = nullptr;
	UINT uCompileFlag = 0;
#ifdef _DEBUG
	uCompileFlag =
		D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
#endif	// #ifdef _DEBUG
	int i = 0;
	m_pPixelShaders.resize(EGageNo_Max);
	for( auto& p : m_pPixelShaders ){
		// HLSL����s�N�Z���V�F�[�_�[�̃u���u���쐬.
		if( FAILED(
			shader::InitShader(
				SHADER_NAME,				// �V�F�[�_�[�t�@�C����.
				PS_SHADER_ENTRY_NAMES[i],	// �V�F�[�_�[�G���g���[�|�C���g.
				"ps_5_0",					// �V�F�[�_�[���f��.
				uCompileFlag,				// �V�F�[�_�[�R���p�C���t���O.
				&pCompiledShader,			// �u���u���i�[���郁�����ւ̃|�C���^.
				&pErrors ))) {				// �G���[�ƌx���ꗗ���i�[���郁�����ւ̃|�C���^.
			ERROR_MESSAGE( (char*)pErrors->GetBufferPointer() );
			return E_FAIL;
		}
		SAFE_RELEASE(pErrors);

		// ��L�ō쐬�����u���u����u�s�N�Z���V�F�[�_�[�v���쐬.
		if( FAILED( shader::CreatePixelShader(
			m_pDevice11,
			pCompiledShader, 
			&m_pPixelShaders[i] ))) {
			ERROR_MESSAGE("�s�N�Z���V�F�[�_�[�쐬 : ���s");
			return E_FAIL;
		}
		i++;
	}
	return S_OK;
}

//---------------------------------------.
// �萔�o�b�t�@�̍쐬.
//---------------------------------------.
HRESULT CCircleGageSprite::InitConstantBuffer()
{
	if( FAILED(
		shader::CreateConstantBuffer( m_pDevice11, &m_pConstantBuffer, sizeof(CBUFFER_PER_FRAME) ))) {
		ERROR_MESSAGE("�萔�o�b�t�@�쐬 : ���s");
		return E_FAIL;
	}
	return S_OK;
}
