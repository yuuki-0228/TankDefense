/**
* @file LeyInput.h
* @brief This is Use Xinput Controller file.
* @author Fukuta
* @date 2020/12/16
* @details Key�����擾���邽�߂̃N���X�ł�.
**/
#ifndef KEY_INPUT_H
#define KEY_INPUT_H

#include <Windows.h>

const int KEY_MAX = 256;	// �L�[�̍ő�l.

/*******************************
* �L�[���͂��擾����N���X.
**/
class CKeyInput
{

public:
	CKeyInput();
	~CKeyInput();

	// �L�[�̏�Ԃ̍X�V.
	static void Update();

	// ������Ă���Ƃ�.
	static bool IsPress			( const unsigned char& key );
	// �����ꂽ�u��.
	static bool IsMomentPress	( const unsigned char& key );
	// ���������Ă���Ƃ�.
	static bool IsHold			( const unsigned char& key );
	// �������u��.
	static bool IsRelease		( const unsigned char& key );

private:
	// �C���X�^���X�̎擾.
	static CKeyInput* GetInstance();

	// �L�[�̏�Ԃ̍X�V.
	void KeyStateUpdate();
	// �L�[�̏�Ԃ̎擾.
	static unsigned char GetNowState( const unsigned char& key );
	// �L�[�̏�Ԃ̎擾.
	static unsigned char GetOldState( const unsigned char& key );

private:
	BYTE m_NowInputState[KEY_MAX];	// ���݂̃L�[�̏��.
	BYTE m_OldInputState[KEY_MAX];	// �O��̃L�[�̏��.

private:
	// �R�s�[�E���[�u�R���X�g���N�^, ������Z�q�̍폜.
	CKeyInput( const CKeyInput & )				= delete;
	CKeyInput( CKeyInput && )					= delete;
	CKeyInput& operator = ( const CKeyInput & )	= delete;
	CKeyInput& operator = ( CKeyInput && )		= delete;
};

#endif	// #ifndef KEY_INPUT_H.