#include "Sprite.h"
#include "..\Shader\Shader.h"
#include "..\..\Utility\FileManager\FileManager.h"
#include "..\..\Object\CameraBase\CameraManager\CameraManager.h"

namespace
{
	const char* SHADER_NAME = "Data\\Shader\\Sprite\\Sprite.hlsl";
	const char* PS_SHADER_ENTRY_NAMES[] =
	{
		"PS_Main",
		"PS_TAlphaOut",
		"PS_TCutOut",
	};
}

CSprite::CSprite()
	: m_SpriteRenderState	()
	, m_pVertexShaderUI		( nullptr )
	, m_pVertexShader3D		( nullptr )
	, m_pPixelShaders		()
	, m_pVertexLayout		( nullptr )
	, m_pConstantBuffer		( nullptr )
	, m_pSampleLinears		()
	, m_pSrcTexture			( nullptr )
	, m_pDestTexture		( nullptr )
	, m_pMaskTexture		( nullptr )
	, m_pVertexBuffer3D		( nullptr )
	, m_pVertexBufferUI		( nullptr )
	, m_SState				()
	, m_Vertices			()
{}

CSprite::~CSprite()
{
	Release();
}

//---------------------------------------.
// ������.
//---------------------------------------.
HRESULT CSprite::Init( ID3D11DeviceContext* pContext11, const char* texturePath )
{
	if( FAILED( SetDevice( pContext11 ) ))			return E_FAIL;
	if( FAILED( VertexInitShader( SHADER_NAME ) ))	return E_FAIL;
	if( FAILED( PixelInitShader() ))				return E_FAIL;
	if( FAILED( InitSample() ))						return E_FAIL;
	if( FAILED( InitConstantBuffer() ))				return E_FAIL;
	if( FAILED( CreateTexture( texturePath ) ))		return E_FAIL;
	if( FAILED( SpriteStateRead( texturePath ) ))	return E_FAIL;
	if( FAILED( InitModel3D() ))					return E_FAIL;
	if( FAILED( InitModelUI() ))					return E_FAIL;

	std::string msg = texturePath;
	msg += " �ǂݍ��� : ����";
	CLog::Print( std::string( msg.c_str() ).c_str() );

	return S_OK;
}

//---------------------------------------.
// ���.
//---------------------------------------.
void CSprite::Release()
{
	SAFE_RELEASE( m_pSrcTexture );
	SAFE_RELEASE( m_pVertexBufferUI );
	SAFE_RELEASE( m_pVertexBuffer3D );

	SAFE_RELEASE( m_pConstantBuffer );
	for( auto& s : m_pSampleLinears )	SAFE_RELEASE( s );
	for( auto& p : m_pPixelShaders )	SAFE_RELEASE( p );
	SAFE_RELEASE( m_pVertexLayout );
	SAFE_RELEASE( m_pVertexShader3D );
	SAFE_RELEASE( m_pVertexShaderUI );

	m_pDestTexture = nullptr;
	m_pMaskTexture = nullptr;

	m_pDevice11 = nullptr;
	m_pContext11 = nullptr;
}

//---------------------------------------.
// �`��.
//---------------------------------------.
void CSprite::Render( SSpriteRenderState* pRenderState )
{
	SSpriteRenderState* renderState = pRenderState == nullptr ? &m_SpriteRenderState : pRenderState;
	SSpriteAnimState* animState = &renderState->AnimState;

	animState->AnimUpdate();		// �A�j���[�V�����̍X�V.
	animState->UVScrollUpdate();	// �X�N���[���̍X�V.

	//�@���[���h�s��.
	D3DXMATRIX mWorld = renderState->Tranceform.GetWorldMatrix();

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

		cb.mWVP	= mWorld * CCameraManager::GetViewProjMatrix();
		cb.mW	= mWorld;
		D3DXMatrixTranspose( &cb.mWVP, &cb.mWVP );
		D3DXMatrixTranspose( &cb.mW, &cb.mW );

		// �r���[�|�[�g�̕�,������n��.
		cb.vViewPort.x	= static_cast<float>(WND_W);
		cb.vViewPort.y	= static_cast<float>(WND_H);

		// �F��n��.
		cb.vColor = renderState->Color;

		// �t�F�[�h�̒l��n��.
		cb.fFadeValue = renderState->FadeValue;

		// �e�N�X�`�����W.
		cb.vUV = animState->UV;

		memcpy_s( pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));

		m_pContext11->Unmap( m_pConstantBuffer, 0 );
	}

	// �V�F�[�_�[�̐ݒ�.
	m_pContext11->VSSetShader( m_pVertexShaderUI, nullptr, 0 );
	m_pContext11->PSSetShader( m_pPixelShaders[renderState->ShaderNo], nullptr, 0 );

	// �R���X�^���g�o�b�t�@�̐ݒ�.
	m_pContext11->VSSetConstantBuffers( 0, 1, &m_pConstantBuffer );
	m_pContext11->PSSetConstantBuffers( 0, 1, &m_pConstantBuffer );

	// ���_���̐ݒ�.
	UINT stride = sizeof(SpriteVertex);
	UINT offset = 0;
	m_pContext11->IASetVertexBuffers( 0, 1, &m_pVertexBufferUI, &stride, &offset );
	m_pContext11->IASetInputLayout( m_pVertexLayout );

	// �摜���̐ݒ�.
	m_pContext11->PSSetSamplers( 0, 1, &m_pSampleLinears[renderState->SmaplerNo] );
	m_pContext11->PSSetShaderResources( 0, 1, &m_pSrcTexture );
	m_pContext11->PSSetShaderResources( 1, 1, &m_pDestTexture );
	m_pContext11->PSSetShaderResources( 2, 1, &m_pMaskTexture );
	m_pContext11->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

	// �`��.
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
void CSprite::Render3D( const bool& isBillboard, SSpriteRenderState* pRenderState )
{
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

		cb.mWVP	= mWorld * CCameraManager::GetViewProjMatrix();
		cb.mW	= mWorld;
		D3DXMatrixTranspose( &cb.mWVP, &cb.mWVP );
		D3DXMatrixTranspose( &cb.mW, &cb.mW );

		// �r���[�|�[�g�̕�,������n��.
		cb.vViewPort.x	= static_cast<float>(WND_W);
		cb.vViewPort.y	= static_cast<float>(WND_H);

		// �F��n��.
		cb.vColor = renderState->Color;

		// �t�F�[�h�̒l��n��.
		cb.fFadeValue = renderState->FadeValue;

		// �e�N�X�`�����W.
		cb.vUV = animState->UV;

		memcpy_s( pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));

		m_pContext11->Unmap( m_pConstantBuffer, 0 );
	}

	// �V�F�[�_�[�̐ݒ�.
	m_pContext11->VSSetShader( m_pVertexShader3D, nullptr, 0 );
	m_pContext11->PSSetShader( m_pPixelShaders[renderState->ShaderNo], nullptr, 0 );

	// �R���X�^���g�o�b�t�@�̐ݒ�.
	m_pContext11->VSSetConstantBuffers( 0, 1, &m_pConstantBuffer );
	m_pContext11->PSSetConstantBuffers( 0, 1, &m_pConstantBuffer );

	// ���_���̐ݒ�.
	UINT stride = sizeof(SpriteVertex);
	UINT offset = 0;
	m_pContext11->IASetVertexBuffers( 0, 1, &m_pVertexBuffer3D, &stride, &offset );
	m_pContext11->IASetInputLayout( m_pVertexLayout );

	// �摜���̐ݒ�.
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
// �V�F�[�_�[�̍쐬.
//---------------------------------------.
HRESULT CSprite::VertexInitShader( const char* shaderPath )
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
			shaderPath,			// �V�F�[�_�[�t�@�C����.
			"VS_MainUI",		// �V�F�[�_�[�G���g���[�|�C���g.
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
		&m_pVertexShaderUI ))) {
		ERROR_MESSAGE("���_�V�F�[�_�[�쐬 : ���s");
		return E_FAIL;
	}

	// HLSL����o�[�e�b�N�X�V�F�[�_�[�̃u���u���쐬.
	if( FAILED(
		shader::InitShader(
			shaderPath,			// �V�F�[�_�[�t�@�C����.
			"VS_Main",			// �V�F�[�_�[�G���g���[�|�C���g.
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
		&m_pVertexShader3D ))) {
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
	if( FAILED(
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

	return S_OK;
}

//---------------------------------------.
// �s�N�Z���V�F�[�_�[�̍쐬.
//---------------------------------------.
HRESULT CSprite::PixelInitShader()
{
	ID3DBlob* pCompiledShader = nullptr;
	ID3DBlob* pErrors = nullptr;
	UINT uCompileFlag = 0;
#ifdef _DEBUG
	uCompileFlag =
		D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
#endif	// #ifdef _DEBUG
	int i = 0;
	m_pPixelShaders.resize(EPSShaderNo_Max);
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
HRESULT CSprite::InitConstantBuffer()
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
HRESULT CSprite::InitSample()
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

//---------------------------------------.
// �X�v���C�g���̓ǂݍ���.
//---------------------------------------.
HRESULT CSprite::SpriteStateRead( const char* filePath )
{
	std::string path = filePath;
	// �g���q��"."�̈ʒu���擾.
	size_t i = path.find(".");
	// �g���q��菜����".txt"�ɕύX.
	path.erase( i, path.size() ) += ".txt";

	std::vector<std::string> pramList = fileManager::TextLoading( path );

	if( pramList.empty() == true ){
		ERROR_MESSAGE( path + "�����݂��܂���" );
		return E_FAIL;
	}

	m_SState.LocalPosNum	= static_cast<ELocalPosition>(std::stoi(pramList[ESpriteStateNo_LocalPosNum]));
	m_SState.Disp.w			= std::stof(pramList[ESpriteStateNo_Disp_w]);
	m_SState.Disp.h			= std::stof(pramList[ESpriteStateNo_Disp_h]);
	m_SState.Base.w			= std::stof(pramList[ESpriteStateNo_Base_w]);
	m_SState.Base.h			= std::stof(pramList[ESpriteStateNo_Base_h]);
	m_SState.Stride.w		= std::stof(pramList[ESpriteStateNo_Stride_w]);
	m_SState.Stride.h		= std::stof(pramList[ESpriteStateNo_Stride_h]);
	m_SState.vPos.x			= std::stof(pramList[ESpriteStateNo_vPos_x]);
	m_SState.vPos.y			= std::stof(pramList[ESpriteStateNo_vPos_y]);
	m_SState.vPos.z			= std::stof(pramList[ESpriteStateNo_vPos_z]);
	m_SState.AnimNum		= std::stoi(pramList[ESpriteStateNo_AnimNum]);

	m_SpriteRenderState.AnimState.MaxAnimNumber = m_SState.AnimNum;
	m_SpriteRenderState.Tranceform.Position = m_SState.vPos;

	return S_OK;
}

//---------------------------------------.
// �e�N�X�`���쐬.
//---------------------------------------.
HRESULT CSprite::CreateTexture( const char* texturePath )
{
	if( FAILED( D3DX11CreateShaderResourceViewFromFile(
		m_pDevice11,	// ���\�[�X���g�p����f�o�C�X�̃|�C���^.
		texturePath,	// �e�N�X�`���p�X��.
		nullptr,
		nullptr,
		&m_pSrcTexture,		// (out)�e�N�X�`��.
		nullptr ))){
		std::string err = texturePath;
		err += " : �e�N�X�`���ǂݍ��� : ���s";
		ERROR_MESSAGE( err );
		return E_FAIL;
	}
	return S_OK;
}

//---------------------------------------.
// ���f���쐬 3D �p.
//---------------------------------------.
HRESULT CSprite::InitModel3D()
{
	// �摜�̔䗦�����߂�.
	int as = myGcd((int)m_SState.Disp.w, (int)m_SState.Disp.h);
	float w = m_SState.Disp.w / as;
	float h = m_SState.Disp.h / as;
	w *= 0.1f;
	h *= 0.1f;
	float u = m_SState.Stride.w / m_SState.Base.w;	// 1�R�}������̕�,
	float v = m_SState.Stride.h / m_SState.Base.h;	// 1�R�}������̍���.

	CreateVertex( w, -h, u, v, true );

	// �T�u���\�[�X�\����.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = m_Vertices;

	// �o�b�t�@�\����.
	D3D11_BUFFER_DESC bd;
	bd.Usage				= D3D11_USAGE_DEFAULT;		// �g�p���@(�f�t�H���g).
	bd.ByteWidth			= sizeof(SpriteVertex) * 4;		// ���_�̃T�C�Y.
	bd.BindFlags			= D3D11_BIND_VERTEX_BUFFER;	// ���_�o�b�t�@�Ƃ��Ĉ���.
	bd.CPUAccessFlags		= 0;						// CPU����̓A�N�Z�X���Ȃ�.
	bd.MiscFlags			= 0;						// ���̑��̃t���O(���g�p).
	bd.StructureByteStride	= 0;						// �\���̂̃T�C�Y(���g�p).

	// ���_�o�b�t�@�̍쐬.
	if( FAILED( m_pDevice11->CreateBuffer( &bd, &InitData, &m_pVertexBuffer3D ))){
		ERROR_MESSAGE( "���_�o�b�t�@�쐬 : ���s" );
		return E_FAIL;
	}

	return S_OK;
}

//---------------------------------------.
// ���f���쐬 UI �p.
//---------------------------------------.
HRESULT CSprite::InitModelUI()
{
	float w = m_SState.Disp.w;	// �\���X�v���C�g��,
	float h = m_SState.Disp.h;	// �\���X�v���C�g����,
	float u = m_SState.Stride.w / m_SState.Base.w;	// 1�R�}������̕�,
	float v = m_SState.Stride.h / m_SState.Base.h;	// 1�R�}������̍���.

	m_SpriteRenderState.AnimState.PatternMax.x = static_cast<LONG>( m_SState.Base.w / m_SState.Stride.w );// x�̍ő�}�X��.
	m_SpriteRenderState.AnimState.PatternMax.y = static_cast<LONG>( m_SState.Base.h / m_SState.Stride.h );// y�̍ő�}�X��.
	//-------------------------------.
	// UI�p.
	//-------------------------------.
	// �o�b�t�@�\����.
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;				// �g�p���@(�f�t�H���g).
	bd.ByteWidth = sizeof(SpriteVertex) * 4;	// ���_�̃T�C�Y.
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// ���_�o�b�t�@�Ƃ��Ĉ���.
	bd.CPUAccessFlags = 0;						// CPU����̓A�N�Z�X���Ȃ�.
	bd.MiscFlags = 0;							// ���̑��̃t���O(���g�p).
	bd.StructureByteStride = 0;					// �\���̂̃T�C�Y(���g�p).

	// �T�u���\�[�X�\����.
	D3D11_SUBRESOURCE_DATA InitData;
	CreateVertex( w, h, u, v, false );
	InitData.pSysMem = m_Vertices;	// �|���̒��_���Z�b�g.

	// ���_�o�b�t�@�̍쐬.
	if( FAILED( 
		m_pDevice11->CreateBuffer( &bd, &InitData, &m_pVertexBufferUI ) )) {
		ERROR_MESSAGE( "���_�o�b�t�@�쐬 : ���s" );
		return E_FAIL;
	}
	return S_OK;
}

//---------------------------------------.
// ���_���̍쐬.
//---------------------------------------.
void CSprite::CreateVertex( const float& w, const float& h, const float& u, const float& v, const bool& is3D )
{
	const float minusH = h * -1.0f;
	switch( m_SState.LocalPosNum )
	{
	case ELocalPosition::LeftUp:
		m_Vertices[0] =
		{ D3DXVECTOR3( 0.0f,	h,		0.0f ), D3DXVECTOR2( 0.0f,	v ) };		// ���_�P(����).
		m_Vertices[1] =
		{ D3DXVECTOR3( 0.0f,	0.0f,	0.0f ), D3DXVECTOR2( 0.0f,	0.0f ) };	// ���_�Q(����).
		m_Vertices[2] =
		{ D3DXVECTOR3( w,		h,		0.0f ), D3DXVECTOR2( u,		v ) };		// ���_�R(�E��).
		m_Vertices[3] =
		{ D3DXVECTOR3( w,		0.0f,	0.0f ), D3DXVECTOR2( u,		0.0f ) };	// ���_�S(�E��).
		break;
	case ELocalPosition::Left:
		m_Vertices[0] =
		{ D3DXVECTOR3( 0.0f,	h / 2,		0.0f ), D3DXVECTOR2( 0.0f,	v ) };		// ���_�P(����).
		m_Vertices[1] =
		{ D3DXVECTOR3( 0.0f,	minusH / 2,	0.0f ), D3DXVECTOR2( 0.0f,	0.0f ) };	// ���_�Q(����).
		m_Vertices[2] =
		{ D3DXVECTOR3( w,		h / 2,		0.0f ), D3DXVECTOR2( u,		v ) };		// ���_�R(�E��).
		m_Vertices[3] =
		{ D3DXVECTOR3( w,		minusH / 2,	0.0f ), D3DXVECTOR2( u,		0.0f ) };	// ���_�S(�E��).
		break;
	case ELocalPosition::LeftDown:
		m_Vertices[0] =
		{ D3DXVECTOR3( 0.0f,	0.0f,		0.0f ), D3DXVECTOR2( 0.0f,	v ) };		// ���_�P(����).
		m_Vertices[1] =
		{ D3DXVECTOR3( 0.0f,	minusH,		0.0f ), D3DXVECTOR2( 0.0f,	0.0f ) };	// ���_�Q(����).
		m_Vertices[2] =
		{ D3DXVECTOR3( w,		0.0f,		0.0f ), D3DXVECTOR2( u,		v ) };		// ���_�R(�E��).
		m_Vertices[3] =
		{ D3DXVECTOR3( w,		minusH,		0.0f ), D3DXVECTOR2( u,		0.0f ) };	// ���_�S(�E��).
		break;
	case ELocalPosition::Down:
		m_Vertices[0] =
		{ D3DXVECTOR3(-w / 2,	0.0f,		0.0f ), D3DXVECTOR2( 0.0f,	v ) };		// ���_�P(����).
		m_Vertices[1] =
		{ D3DXVECTOR3(-w / 2,	minusH,		0.0f ), D3DXVECTOR2( 0.0f,	0.0f ) };	// ���_�Q(����).
		m_Vertices[2] =
		{ D3DXVECTOR3( w / 2,	0.0f,		0.0f ), D3DXVECTOR2( u,		v ) };		// ���_�R(�E��).
		m_Vertices[3] =
		{ D3DXVECTOR3( w / 2,	minusH,		0.0f ), D3DXVECTOR2( u,		0.0f ) };	// ���_�S(�E��).
		break;
	case ELocalPosition::RightDown:
		m_Vertices[0] =
		{ D3DXVECTOR3(-w,		0.0f,		0.0f ), D3DXVECTOR2( 0.0f,	v ) };		// ���_�P(����).
		m_Vertices[1] =
		{ D3DXVECTOR3(-w,		minusH,		0.0f ), D3DXVECTOR2( 0.0f,	0.0f ) };	// ���_�Q(����).
		m_Vertices[2] =
		{ D3DXVECTOR3( 0.0f,	0.0f,		0.0f ), D3DXVECTOR2( u,		v ) };		// ���_�R(�E��).
		m_Vertices[3] =
		{ D3DXVECTOR3( 0.0f,	minusH,		0.0f ), D3DXVECTOR2( u,		0.0f ) };	// ���_�S(�E��).
		break;
	case ELocalPosition::Right:
		m_Vertices[0] =
		{ D3DXVECTOR3(-w,		h / 2,		0.0f ), D3DXVECTOR2( 0.0f,	v ) };		// ���_�P(����).
		m_Vertices[1] =
		{ D3DXVECTOR3(-w,		minusH / 2,	0.0f ), D3DXVECTOR2( 0.0f,	0.0f ) };	// ���_�Q(����).
		m_Vertices[2] =
		{ D3DXVECTOR3( 0.0f,	h / 2,		0.0f ), D3DXVECTOR2( u,		v ) };		// ���_�R(�E��).
		m_Vertices[3] =
		{ D3DXVECTOR3( 0.0f,	minusH / 2,	0.0f ), D3DXVECTOR2( u,		0.0f ) };	// ���_�S(�E��).
		break;
	case ELocalPosition::RightUp:
		m_Vertices[0] =
		{ D3DXVECTOR3(-w,		h,		0.0f ), D3DXVECTOR2( 0.0f,	v ) };		// ���_�P(����).
		m_Vertices[1] =
		{ D3DXVECTOR3(-w,		0.0f,	0.0f ), D3DXVECTOR2( 0.0f,	0.0f ) };	// ���_�Q(����).
		m_Vertices[2] =
		{ D3DXVECTOR3( 0.0f,	h,		0.0f ), D3DXVECTOR2( u,		v ) };		// ���_�R(�E��).
		m_Vertices[3] =
		{ D3DXVECTOR3( 0.0f,	0.0f,	0.0f ), D3DXVECTOR2( u,		0.0f ) };	// ���_�S(�E��).
		break;
	case ELocalPosition::Up:
		m_Vertices[0] =
		{ D3DXVECTOR3(-w / 2,	h,		0.0f ), D3DXVECTOR2( 0.0f,	v) };		// ���_�P(����).
		m_Vertices[1] =
		{ D3DXVECTOR3(-w / 2,	0.0f,	0.0f ), D3DXVECTOR2( 0.0f,	0.0f) };	// ���_�Q(����).
		m_Vertices[2] =
		{ D3DXVECTOR3( w / 2,	h,		0.0f ), D3DXVECTOR2( u,		v) };		// ���_�R(�E��).
		m_Vertices[3] =
		{ D3DXVECTOR3( w / 2,	0.0f,	0.0f ), D3DXVECTOR2( u,		0.0f) };	// ���_�S(�E��).
		break;
	case ELocalPosition::Center:
	default:
		m_Vertices[0] =
		{ D3DXVECTOR3(-w / 2,	h / 2,		0.0f ), D3DXVECTOR2( 0.0f,	v ) };		// ���_�P(����).
		m_Vertices[1] =
		{ D3DXVECTOR3(-w / 2,	minusH / 2,	0.0f ), D3DXVECTOR2( 0.0f,	0.0f ) };	// ���_�Q(����).
		m_Vertices[2] =
		{ D3DXVECTOR3( w / 2,	h / 2,		0.0f ), D3DXVECTOR2( u,		v ) };		// ���_�R(�E��).
		m_Vertices[3] =
		{ D3DXVECTOR3( w / 2,	minusH / 2,	0.0f ), D3DXVECTOR2( u,		0.0f ) };	// ���_�S(�E��).
		break;
	}
}