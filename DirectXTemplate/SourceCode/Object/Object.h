/**
* @file Objct.h.
* @brief オブジェクト基底クラス.
* @author 福田玲也.
*/
#ifndef OBJECT_H
#define OBJECT_H

#include "..\Global.h"
#include "..\Common\DebugText\DebugText.h"

/*******************************************
*	オブジェクトクラス.
**/
class CObject
{
public:
	CObject();
	virtual ~CObject();

	// トランスフォームの設定.
	inline void SetTranceform	( const STranceform& t ){ m_Tranceform = t; }
	// 座標の設定.
	inline void SetPosition		( const D3DXVECTOR3& p ){ m_Tranceform.Position = p; }
	// 回転の設定.
	inline void SetRotation		( const D3DXVECTOR3& r ){ m_Tranceform.Rotation = r; }
	// 拡大・縮小の設定.
	inline void SetScale		( const D3DXVECTOR3& s ){ m_Tranceform.Scale = s; }

	// トランスフォームの取得.
	inline STranceform GetTranceform() const { return m_Tranceform; }
	// 座標の取得.
	inline D3DXVECTOR3 GetPosition	() const { return m_Tranceform.Position; }
	// 回転の取得.
	inline D3DXVECTOR3 GetRotation	() const { return m_Tranceform.Rotation; }
	// 拡大・縮小の取得.
	inline D3DXVECTOR3 GetScale		() const { return m_Tranceform.Scale; }

protected:
	STranceform m_Tranceform;	// トランスフォーム.
};

#endif	// #ifndef OBJECT_H.