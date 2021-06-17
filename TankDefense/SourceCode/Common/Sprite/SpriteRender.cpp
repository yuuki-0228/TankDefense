#include "SpriteRender.h"
#include "Sprite.h"
#include "..\Shader\Shader.h"

namespace
{
	const char* SHADER_NAME = "Data\\Shader\\Sprite\\Sprite.hlsl";
};

CSpriteRender::CSpriteRender()
	: m_pSpriteQueue	()
	, m_pVertexShader	( nullptr )
	, m_pPixelShader	( nullptr )
	, m_pVertexLayout	( nullptr )
	, m_pConstantBuffer	( nullptr )
	, m_pSampleLinears	()
{
}

CSpriteRender::~CSpriteRender()
{
	Release();
}

//---------------------------------------.
// ������.
//---------------------------------------.
HRESULT CSpriteRender::Init( ID3D11DeviceContext* pContext11 )
{
	// �f�o�C�X�̎擾.
	if( FAILED( SetDevice(pContext11) ))	return E_FAIL;
	if( FAILED( InitShader() ))				return E_FAIL;
	if( FAILED( InitSample() ))				return E_FAIL;
	if( FAILED( InitConstantBuffer() ))		return E_FAIL;

	CLog::Print( "�摜�`��N���X�쐬 : ����" );

	return S_OK;
}

//---------------------------------------.
// ���.
//---------------------------------------.
void CSpriteRender::Release()
{
	const int queueSize = static_cast<int>(m_pSpriteQueue.size());
	for( int i = 0; i < queueSize; i++ ){
		m_pSpriteQueue.front().first = nullptr;
		m_pSpriteQueue.pop();
	}

	SAFE_RELEASE( m_pConstantBuffer );
	for( auto& s : m_pSampleLinears ) SAFE_RELEASE( s );
	SAFE_RELEASE( m_pPixelShader );
	SAFE_RELEASE( m_pVertexLayout );
	SAFE_RELEASE( m_pVertexShader );
	
	m_pDevice11 = nullptr;
	m_pContext11 = nullptr;
}

//---------------------------------------.
// �`��.
//---------------------------------------.
void CSpriteRender::Render()
{
	const int queueSize = static_cast<int>(m_pSpriteQueue.size());
	for( int i = 0; i < queueSize; i++ ){
		const CSprite*				pSprite		= m_pSpriteQueue.front().first;
		const SSpriteRenderState	renderState	= m_pSpriteQueue.front().second;
		if( pSprite == nullptr ) continue;

		// �摜���̎擾.
		ID3D11Buffer*				pVertexBuffer	= pSprite->GetVertexBufferUI();
		ID3D11ShaderResourceView*	pTexture		= pSprite->GetTexture();
	
		Render( renderState, pTexture, pVertexBuffer );

		m_pSpriteQueue.pop();
	}
}

//---------------------------------------.
// �`��.
//---------------------------------------.
void CSpriteRender::Render( 
	const SSpriteRenderState& renderState,
	ID3D11ShaderResourceView* pTexture,
	ID3D11Buffer* pVertexBufferUI )
{
	const SSpriteAnimState animState	= renderState.AnimState;
	//�@���[���h�s��.
	D3DXMATRIX mWorld = renderState.Tranceform.GetWorldMatrix();

	// �V�F�[�_�[�̃R���X�^���g�o�b�t�@�Ɋe��f�[�^��n��.
	D3D11_MAPPED_SUBRESOURCE pData;
	C_BUFFER cb;	// �R���X�^���g�o�b�t�@.

	// �o�b�t�@���̃f�[�^�̏��������J�n����Map.
	if( SUCCEEDED( m_pContext11->Map(
		m_pConstantBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&pData ))){

		cb.mWVP	= mWorld;
		cb.mW	= mWorld;
		D3DXMatrixTranspose( &cb.mWVP, &cb.mWVP );
		D3DXMatrixTranspose( &cb.mW, &cb.mW );

		// �r���[�|�[�g�̕�,������n��.
		cb.vViewPort.x	= static_cast<float>(WND_W);
		cb.vViewPort.y	= static_cast<float>(WND_H);

		// �F��n��.
		cb.vColor = renderState.Color;

		// �e�N�X�`�����W.
		cb.vUV = animState.UV;

		memcpy_s( pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));

		m_pContext11->Unmap( m_pConstantBuffer, 0 );
	}

	// �V�F�[�_�[�̐ݒ�.
	m_pContext11->VSSetShader( m_pVertexShader, nullptr, 0 );
	m_pContext11->PSSetShader( m_pPixelShader, nullptr, 0 );

	// �R���X�^���g�o�b�t�@�̐ݒ�.
	m_pContext11->VSSetConstantBuffers( 0, 1, &m_pConstantBuffer );
	m_pContext11->PSSetConstantBuffers( 0, 1, &m_pConstantBuffer );

	// ���_���̐ݒ�.
	UINT stride = sizeof(SpriteVertex);
	UINT offset = 0;
	m_pContext11->IASetVertexBuffers( 0, 1, &pVertexBufferUI, &stride, &offset );
	m_pContext11->IASetInputLayout( m_pVertexLayout );

	// �摜���̐ݒ�.
	m_pContext11->PSSetSamplers( 0, 1, &m_pSampleLinears[renderState.SmaplerNo] );
	m_pContext11->PSSetShaderResources( 0, 1, &pTexture );
	m_pContext11->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

	// �`��.
	SetBlend( true );
	SetDeprh( false );
	SetRasterizerState( ERS_STATE::Back );
	m_pContext11->Draw( 4, 0 );
	SetRasterizerState( ERS_STATE::None );
	SetDeprh( true );
	SetBlend( false );
}

//---------------------------------------.
// �摜�f�[�^�̒ǉ�.
//---------------------------------------.
void CSpriteRender::PushSpriteData( const spriteRenderPair& renderPair )
{
	m_pSpriteQueue.push( renderPair );
}
void CSpriteRender::PushSpriteData( CSprite* pSprite, const SSpriteRenderState& renderState )
{
	m_pSpriteQueue.push( { pSprite, renderState } );
}
void CSpriteRender::PushSpriteData( CSprite* pSprite )
{
	SSpriteRenderState state;
	state.Tranceform.Position = pSprite->GetRenderPos();
	state.AnimState = pSprite->GetAnimState();
	PushSpriteData( pSprite, state );
}

//---------------------------------------.
// �V�F�[�_�[�̍쐬.
//---------------------------------------.
HRESULT CSpriteRender::InitShader()
{
	ID3DBlob* pCompiledShader = nullptr;
	ID3DBlob* pErrors = nullptr;
	UINT uCompileFlag = 0;
#ifdef _DEBUG
	uCompileFlag =
		D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
#endif	// #ifdef _DEBUG

	// HLSL����o�[�e�b�N�X�V�F�[�_�[�̃u���u���쐬.
	if( FAILED(
		shader::InitShader(
			SHADER_NAME,		// �V�F�[�_�[�t�@�C����.
			"VS_MainUI",			// �V�F�[�_�[�G���g���[�|�C���g.
			"vs_5_0",			// �V�F�[�_�[���f��.
			uCompileFlag,		// �V�F�[�_�[�R���p�C���t���O.
			&pCompiledShader,	// �u���u���i�[���郁�����ւ̃|�C���^.
			&pErrors ))) {		// �G���[�ƌx���ꗗ���i�[���郁�����ւ̃|�C���^.
		ERROR_MESSAGE( (char*)pErrors->GetBufferPointer() );
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);

	// ��L�ō쐬�����u���u����u���_�V�F�[�_�[�v���쐬.
	if( FAILED( shader::CreateVertexShader(
		m_pDevice11,
		pCompiledShader, 
		&m_pVertexShader ))) {
		ERROR_MESSAGE("���_�V�F�[�_�[�쐬 : ���s");
		return E_FAIL;
	}

	// ���_�C���v�b�g���C�A�E�g���`.
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		shader::GetPositionInputElement(),
		shader::GetTexcoordInputElement(),
	};

	// ���_�C���v�b�g���C�A�E�g�̔z��v�f�����Z�o.
	UINT numElements = sizeof(layout) / sizeof(layout[0]);

	// ���_�C���v�b�g���C�A�E�g���쐬.
	if (FAILED(
		shader::CreateInputLayout(
			m_pDevice11,
			layout,
			numElements,
			pCompiledShader,
			&m_pVertexLayout))) {
		ERROR_MESSAGE("���_�C���v�b�g���C�A�E�g�쐬 : ���s");
		return E_FAIL;
	}
	SAFE_RELEASE(pCompiledShader);


	// HLSL����s�N�Z���V�F�[�_�[�̃u���u���쐬.
	if( FAILED(
		shader::InitShader(
			SHADER_NAME,		// �V�F�[�_�[�t�@�C����.
			"PS_Main",			// �V�F�[�_�[�G���g���[�|�C���g.
			"ps_5_0",			// �V�F�[�_�[���f��.
			uCompileFlag,		// �V�F�[�_�[�R���p�C���t���O.
			&pCompiledShader,	// �u���u���i�[���郁�����ւ̃|�C���^.
			&pErrors ))) {		// �G���[�ƌx���ꗗ���i�[���郁�����ւ̃|�C���^.
		ERROR_MESSAGE( (char*)pErrors->GetBufferPointer() );
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);

	// ��L�ō쐬�����u���u����u�s�N�Z���V�F�[�_�[�v���쐬.
	if( FAILED( shader::CreatePixelShader(
		m_pDevice11,
		pCompiledShader, 
		&m_pPixelShader ))) {
		ERROR_MESSAGE("�s�N�Z���V�F�[�_�[�쐬 : ���s");
		return E_FAIL;
	}

	return S_OK;
}

//---------------------------------------.
// �萔�o�b�t�@�̍쐬.
//---------------------------------------.
HRESULT CSpriteRender::InitConstantBuffer()
{
	if( FAILED(
		shader::CreateConstantBuffer( m_pDevice11, &m_pConstantBuffer, sizeof(C_BUFFER) ))) {
		ERROR_MESSAGE("�萔�o�b�t�@�쐬 : ���s");
		return E_FAIL;
	}
	return S_OK;
}

//---------------------------------------.
// �T���v���̍쐬.
//---------------------------------------.
HRESULT CSpriteRender::InitSample()
{
	// �e�N�X�`���p�̃T���v���\����.
	D3D11_SAMPLER_DESC samDesc;
	ZeroMemory( &samDesc, sizeof(samDesc) );
	samDesc.Filter		= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samDesc.AddressU	= D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.AddressV	= D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.AddressW	= D3D11_TEXTURE_ADDRESS_WRAP;

	// �T���v���쐬.
	for( int i = 0; i < ESamplerState_Max; i++ ){
		samDesc.AddressU = static_cast<D3D11_TEXTURE_ADDRESS_MODE>(i+1);
		samDesc.AddressV = static_cast<D3D11_TEXTURE_ADDRESS_MODE>(i+1);
		samDesc.AddressW = static_cast<D3D11_TEXTURE_ADDRESS_MODE>(i+1);
		if( FAILED( m_pDevice11->CreateSamplerState( &samDesc, &m_pSampleLinears[i] ))){
			ERROR_MESSAGE("�T���v���쐬 : ���s");
			return E_FAIL;
		}
	}

	return S_OK;
}