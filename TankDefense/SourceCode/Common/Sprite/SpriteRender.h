/**
* @file SpriteRender.h.
* @brief �X�v���C�g�`��N���X.
* @author ���c���.
*/
#ifndef SPRITE_RENDER_H
#define SPRITE_RENDER_H

#include "..\Common.h"
#include "SpriteStruct.h"
#include <queue>

class CSprite;

using spriteRenderPair = std::pair<CSprite*, SSpriteRenderState>;

/***************************************
*	�摜�`��N���X.
*		�摜����CSprite�N���X�������Ă���.
**/
class CSpriteRender : public CCommon
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
	};
public:
	CSpriteRender();
	virtual ~CSpriteRender();

	// ������.
	HRESULT Init( ID3D11DeviceContext* pContext11 );

	// ���.
	void Release();

	// �`��.
	void Render();
	void Render(
		const SSpriteRenderState& renderState, 
		ID3D11ShaderResourceView* pTexture, 
		ID3D11Buffer* pVertexBufferUI );

	// �摜�f�[�^�̒ǉ�.
	void PushSpriteData( const spriteRenderPair& renderPair );
	void PushSpriteData( CSprite* pSprite, const SSpriteRenderState& renderState );
	void PushSpriteData( CSprite* pSprite );

private:
	// �V�F�[�_�[�̍쐬.
	HRESULT InitShader();
	// �萔�o�b�t�@�̍쐬.
	HRESULT InitConstantBuffer();
	// �T���v���̍쐬.
	HRESULT InitSample();

private:
	std::queue<spriteRenderPair>	m_pSpriteQueue;							// �摜�L���[.
	ID3D11VertexShader*				m_pVertexShader;						// ���_�V�F�[�_�[.
	ID3D11PixelShader*				m_pPixelShader;							// �s�N�Z���V�F�[�_�[.
	ID3D11InputLayout*				m_pVertexLayout;						// ���_���C�A�E�g.
	ID3D11Buffer*					m_pConstantBuffer;						// �R���X�^���g�o�b�t�@.
	ID3D11SamplerState*				m_pSampleLinears[ESamplerState_Max];	// �T���v��:�e�N�X�`���Ɋe��t�B���^��������.
};

#endif	// #ifndef SPRITE_RENDER_H.