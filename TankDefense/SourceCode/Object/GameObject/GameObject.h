/**
* @file GameObject.h.
* @brief ゲーム内のオブジェクト基底クラス.
* @author 福田玲也.
*/
#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "..\Object.h"
#include "GameObjectList.h"

/*************************************
*	ゲームオブジェクト.
*	　ゲーム内オブジェクト、UIなどの継承させるクラス.
**/
class CGameObject : public CObject
{
	// ウィンドウ外の調整値.
	static constexpr float WND_OUT_ADJ_SIZE = 300.0f;
public:
	CGameObject();
	virtual ~CGameObject();

	// 初期化関数.
	virtual bool Init() = 0;
	// 更新関数.
	virtual void Update( const float& deltaTime ) = 0;
	// 描画関数.
	virtual void Render() = 0;

	// ゲームオブジェクトタグの取得.
	inline EObjectTag GetObjectTag(){ return m_ObjectTag; }

	// 画面の外に出ているか.
	bool IsDisplayOut( const float& adjSize = WND_OUT_ADJ_SIZE );

protected:
	// ゲームオブジェクトタグの設定.
	inline void SetObjectTag( const EObjectTag& tag ){ m_ObjectTag = tag; }

protected:
	float		m_DeltaTime;	// デルタタイム(必要ならアップデート関数の始めで設定する).
	EObjectTag	m_ObjectTag;	// オブジェクトタグ.
};

#endif	// #ifndef GAME_OBJECT_H.
