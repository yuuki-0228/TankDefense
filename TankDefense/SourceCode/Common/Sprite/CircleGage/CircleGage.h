#ifndef CIRCLE_GAGE_H
#define CIRCLE_GAGE_H

#include "..\Sprite.h"

/**************************************
*	�Q�[�W�X�v���C�g�p�N���X.
**/
class CCircleGageSprite : public CSprite
{
	struct CBUFFER_PER_FRAME
	{
		ALIGN16 D3DXMATRIX	mW;				// ���[���h�s��.
		ALIGN16 D3DXMATRIX	mWVP;			// ���[���h�s��.
		ALIGN16 D3DXVECTOR4 vColor;			// �F.
		ALIGN16 D3DXVECTOR2	vUV;			// UV���W.
		ALIGN16 D3DXVECTOR2	vViewPort;		// �r���[�|�[�g.
		ALIGN16 D3DXVECTOR2	vCenterPos;		// �~�̒��S���W : 0~1.
		ALIGN16 D3DXVECTOR2	vStartVector;	// �~�̊J�n�x�N�g��.
		ALIGN16 float		Value;			// �l.
		ALIGN16 float		IsClockwise;	// ���v���肩.
	};

public:
	// �Q�[�W�̎��.
	enum enGageNo : unsigned char
	{
		EGageNo_None,

		EGageNo_Circle = 0,	// �T�[�N���Q�[�W.

		EGageNo_Max,
	} typedef EGageNo;

public:
	CCircleGageSprite();
	virtual ~CCircleGageSprite();

	// ������.
	virtual HRESULT Init( ID3D11DeviceContext* pContext11 = nullptr, const char* texturePath = nullptr ) override;

	// ���.
	void Release();

	// �`��.
	virtual void Render( SSpriteRenderState* pRenderState = nullptr ) override;
	// 3D�`��.
	virtual void Render3D( const bool& isBillboard = false, SSpriteRenderState* pRenderState = nullptr ) override;

	// �Z���^�[���W�̐ݒ�.
	//	0 ~ 1�̒l.
	//	�X�v���C�g�̃��[�J�����W�����S�ŁA
	//	�X�v���C�g�̒��S�ɃT�[�N����u�������ꍇ�A
	//	���W��(0.5, 0.5)�ɐݒ肷��Ηǂ�.
	inline void SetCenterPosition( const D3DXVECTOR2& pos ){ m_CenterPosition = pos; }

	// �T�[�N���̊J�n�x�N�g���̐ݒ�.
	//	-1 ~ 1�̒l.
	//	�ォ��J�n�������ꍇ��(0,-1)�ɐݒ肷��.
	inline void SetStartVector( const D3DXVECTOR2& vec ){ m_StartVector = vec; }

	// �l�̐ݒ� : 0 ~ 1 �̒l.
	inline void SetValue( const float& value ){ m_Value = value; }

	// �T�[�N���̊p�x�̐ݒ�.
	//	0 ~ 360�̒l.
	inline void SetAngleDgree( const float& deg )	{ m_Angle = static_cast<float>(D3DXToRadian(deg)); }
	// �T�[�N���̊p�x�̐ݒ�.
	//	0 ~ PI*2�̒l.
	inline void SetAngleRadian( const float& rad )	{ m_Angle = rad; }

	// ���v���肩�ǂ����ݒ�.
	inline void SetIsClockwise( const bool& isClockwise )	{ m_IsClockwise = isClockwise; }

	// �e�N�X�`���̐ݒ�.
	inline void SetSrcTexture( ID3D11ShaderResourceView* pTexture ){ m_pSrcTexture = pTexture; }
	// ���_�o�b�t�@�̐ݒ�.
	inline void SetVertexBufferUI( ID3D11Buffer* pBuffer ){ m_pVertexBufferUI = pBuffer; }
	// ���_�o�b�t�@�̐ݒ�.
	inline void SetVertexBuffer3D( ID3D11Buffer* pBuffer ){ m_pVertexBuffer3D = pBuffer; }

private:
	// �s�N�Z���V�F�[�_�[�̍쐬.
	virtual HRESULT PixelInitShader() override;
	// �萔�o�b�t�@�̍쐬.
	virtual HRESULT InitConstantBuffer()override;

private:
	D3DXVECTOR2		m_CenterPosition;	// �~�̒��S���W.
	D3DXVECTOR2		m_StartVector;		// �J�n�x�N�g��.
	float			m_Value;			// �Q�[�W�̒l.
	float			m_Angle;			// �T�[�N���̊p�x(���W�A��).
	bool			m_IsClockwise;		// ���v��肩.
};

#endif	// #ifndef CIRCLE_GAGE_H.