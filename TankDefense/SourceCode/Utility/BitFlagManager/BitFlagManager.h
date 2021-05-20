/**
* @file BitFlagManager.h.
* @brief �r�b�g�Ǘ��ł���֐����܂Ƃ߂��w�b�_�[.
* @author ���c���.
*/
#ifndef BIT_FLAG_MANAGER_H
#define BIT_FLAG_MANAGER_H

namespace bit
{
	// �r�b�g�𗧂Ă�.
	template<typename T>
	void OnBitFlag( T* pBit, const unsigned int& bitFlag )
	{
		(*pBit) |= bitFlag;
	}
	// �r�b�g�����낷.
	template<typename T>
	void OffBitFlag( T* pBit, const unsigned int& bitFlag )
	{
		(*pBit) &= ~bitFlag;
	}
	// �r�b�g������������.
	template<typename T>
	void CleanBitFlag( T* pBit )
	{
		(*pBit) = 0;
	}
	// �r�b�g�������Ă��邩.
	template<typename T>
	bool IsBitFlag( const T& bit, const unsigned int& bitFlag )
	{
		return (bit & bitFlag);
	}

}

#endif	// #ifndef BIT_FLAG_MANAGER_H.