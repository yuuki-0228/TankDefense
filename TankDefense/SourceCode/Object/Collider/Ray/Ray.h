/**
* @file Ray.h.
* @brief レイクラス.
* @author 福田玲也.
*/
#ifndef RAY_H
#define RAY_H

#include "..\ColliderBase.h"

/*****************************************
*	レイクラス.
**/
class CRay : public CColliderBase
{
public:
	CRay();
	virtual ~CRay();

	// 方向ベクトルの設定.
	void SetVector( const D3DXVECTOR3& v )		{ m_Vector = v; }
	// 開始位置の設定.
	void SetStartPos( const D3DXVECTOR3& pos )	{ m_StartPos = pos; }
	// 終了位置の設定.
	void SetEndPos( const D3DXVECTOR3& pos )	{ m_EndPos = pos ; }
	// 方向ベクトルの取得.
	D3DXVECTOR3 GetVector()		const { return m_Vector; }
	// 開始位置の取得.
	D3DXVECTOR3 GetStartPos()	const { return m_StartPos; }
	// 終了位置の取得.
	D3DXVECTOR3 GetEndPos()		const { return m_EndPos; }

private:
	D3DXVECTOR3 m_Vector;	// 方向ベクトル.
	D3DXVECTOR3 m_StartPos;	// レイ開始位置.
	D3DXVECTOR3 m_EndPos;	// レイ終了位置.
};

#endif	// #ifndef RAY_H.