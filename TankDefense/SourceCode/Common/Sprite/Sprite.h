/**
* @file Sprite.h.
* @brief �X�v���C�g�N���X.
* @author ���c���.
*/
#ifndef SPRITE_H
#define SPRITE_H

#include "..\Common.h"
#include "SpriteStruct.h"

/****************************************
*	�X�v���C�g�N���X.
**/
class CSprite : public CCommon
{
	// �R���X�^���g�o�b�t�@�̃A�v�����̒�`.
	//���V�F�[�_�[���̃R���X�^���g�o�b�t�@�ƈ�v���Ă���K�v����.
	struct C_BUFFER
	{
		ALIGN16 D3DXMATRIX	mW;				// ���[���h�s��.
		ALIGN16 D3DXMATRIX	mWVP;			// ���[���h�E�r���[�E�v���W�F�N�V����.
		ALIGN16 D3DXVECTOR4 vColor;			// �J���[(RGBA�̌^�ɍ��킹��).
		ALIGN16 D3DXVECTOR2 vUV;			// UV���W.
		ALIGN16 D3DXVECTOR2 vViewPort;		// �r���[�|�[�g.
		ALIGN16 float		fFadeValue;		// �t�F�[�h�A�g�����W�V�����̒l.
	};
public:
	CSprite();
	virtual ~CSprite();

	// ������.
	virtual HRESULT Init( ID3D11DeviceContext* pContext11, const char* texturePath = nullptr );

	// ���.
	void Release();

	// �`��.
	virtual void Render( SSpriteRenderState* pRenderState = nullptr );
	// 3D�`��.
	virtual void Render3D( const bool& isBillboard = false, SSpriteRenderState* pRenderState = nullptr );

	// �e�N�X�`���̎擾.
	inline ID3D11ShaderResourceView*	GetTexture()		const { return m_pSrcTexture; }
	// �}�X�N�e�N�X�`���̐ݒ�.
	inline void SetDestTexture( ID3D11ShaderResourceView* pTexture ){ m_pDestTexture = pTexture; }
	// �}�X�N�e�N�X�`���̐ݒ�.
	inline void SetMaskTexture( ID3D11ShaderResourceView* pTexture ){ m_pMaskTexture = pTexture; }

	// ���_�o�b�t�@�̎擾.
	inline ID3D11Buffer*				GetVertexBufferUI()	const { return m_pVertexBufferUI; }
	// ���_�o�b�t�@�̎擾.
	inline ID3D11Buffer*				GetVertexBuffer3D()	const { return m_pVertexBuffer3D; }
	// �`����W�̎擾.
	inline D3DXVECTOR3					GetRenderPos()		const { return m_SState.vPos; }
	// �摜�T�C�Y�̎擾.
	inline WHDIZE_FLOAT					GetTextureSize()	const { return m_SState.Disp; }
	// �e�N�X�`�����W�̎擾.
	inline SSpriteAnimState				GetAnimState()		const { return m_SpriteRenderState.AnimState; }
	// �摜�̕`������擾.
	inline SSpriteRenderState			GetRenderState()	const { return m_SpriteRenderState; }

	// �A�j���[�V�����̍X�V�����邩.
	inline void SetAnimUpdate( const bool& isUpdate ){ m_SpriteRenderState.AnimState.IsAnimation = isUpdate; }
	// �A�j���[�V�����̉��Z�t���[����ݒ�.
	inline void SetAnimFrame( const int& frame ){ m_SpriteRenderState.AnimState.FrameCountSpeed = frame; }

	// X������UV�X�N���[���̍X�V�����邩.
	inline void SetScrollUpdateX( const bool& isUpdate ){ m_SpriteRenderState.AnimState.IsUVScrollX = isUpdate; }
	// Y������UV�X�N���[���̍X�V�����邩.
	inline void SetScrollUpdateY( const bool& isUpdate ){ m_SpriteRenderState.AnimState.IsUVScrollY = isUpdate; }

	// X������UV�X�N���[���̑��x��ݒ�.
	inline void SetScrollSpeedX( const float& speed ){ m_SpriteRenderState.AnimState.ScrollSpeed.x = speed; }
	// Y������UV�X�N���[���̑��x��ݒ�.
	inline void SetScrollSpeedY( const float& speed ){ m_SpriteRenderState.AnimState.ScrollSpeed.y = speed; }

protected:
	// ���_�V�F�[�_�[�̍쐬.
	HRESULT VertexInitShader( const char* shaderPath = nullptr );
	// �s�N�Z���V�F�[�_�[�̍쐬.
	virtual HRESULT PixelInitShader();
	// �萔�o�b�t�@�̍쐬.
	virtual HRESULT InitConstantBuffer();
	// �T���v���̍쐬.
	HRESULT InitSample();

	// �X�v���C�g���̓ǂݍ���.
	HRESULT SpriteStateRead( const char* filePath );
	// �e�N�X�`���쐬.
	HRESULT CreateTexture( const char* texturePath );
	// ���f���쐬 3D �p.
	HRESULT InitModel3D();
	// ���f���쐬 UI �p.
	HRESULT InitModelUI();

	// ���_���̍쐬.
	void CreateVertex(
		const float& w, const float& h,
		const float& u, const float& v, const bool& is3D );

	// �e�N�X�`���̔䗦���擾.
	int myGcd( int t, int t2 ) { if (t2 == 0) return t; return myGcd(t2, t % t2); }

protected:
	SSpriteRenderState				m_SpriteRenderState;
	ID3D11VertexShader*				m_pVertexShaderUI;						// ���_�V�F�[�_�[.
	ID3D11VertexShader*				m_pVertexShader3D;						// ���_�V�F�[�_�[.
	std::vector<ID3D11PixelShader*>	m_pPixelShaders;						// �s�N�Z���V�F�[�_�[.
	ID3D11InputLayout*				m_pVertexLayout;						// ���_���C�A�E�g.
	ID3D11Buffer*					m_pConstantBuffer;						// �R���X�^���g�o�b�t�@.
	ID3D11SamplerState*				m_pSampleLinears[ESamplerState_Max];	// �T���v��:�e�N�X�`���Ɋe��t�B���^��������.

	ID3D11ShaderResourceView*	m_pSrcTexture;	// �ʏ�e�N�X�`��.
	ID3D11ShaderResourceView*	m_pDestTexture;	// �e�N�X�`��.
	ID3D11ShaderResourceView*	m_pMaskTexture;	// �e�N�X�`��.

	ID3D11Buffer*		m_pVertexBuffer3D;	// ���_�o�b�t�@.
	ID3D11Buffer*		m_pVertexBufferUI;	// ���_�o�b�t�@.

	SSpriteState		m_SState;			// �X�v���C�g���.

	SpriteVertex	m_Vertices[4];			// ���_�쐬�p.
};

#endif	// #ifndef SPRITE_H.
