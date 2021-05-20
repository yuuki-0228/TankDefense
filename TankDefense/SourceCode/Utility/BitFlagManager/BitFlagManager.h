/**
* @file BitFlagManager.h.
* @brief ビット管理できる関数をまとめたヘッダー.
* @author 福田玲也.
*/
#ifndef BIT_FLAG_MANAGER_H
#define BIT_FLAG_MANAGER_H

namespace bit
{
	// ビットを立てる.
	template<typename T>
	void OnBitFlag( T* pBit, const unsigned int& bitFlag )
	{
		(*pBit) |= bitFlag;
	}
	// ビットを下ろす.
	template<typename T>
	void OffBitFlag( T* pBit, const unsigned int& bitFlag )
	{
		(*pBit) &= ~bitFlag;
	}
	// ビットを初期化する.
	template<typename T>
	void CleanBitFlag( T* pBit )
	{
		(*pBit) = 0;
	}
	// ビットが立っているか.
	template<typename T>
	bool IsBitFlag( const T& bit, const unsigned int& bitFlag )
	{
		return (bit & bitFlag);
	}

}

#endif	// #ifndef BIT_FLAG_MANAGER_H.