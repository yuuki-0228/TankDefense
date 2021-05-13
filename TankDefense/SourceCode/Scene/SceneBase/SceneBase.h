/**
* @file SceneBase.h.
* @brief シーン基底クラス.
* @author 福田玲也.
*/
#ifndef SCENE_BASE_H
#define SCENE_BASE_H

#include <Windows.h>
#include <string>
#include <stack>
#include <memory>

#include "..\..\Global.h"
#include "..\..\Object\Fade\Fade.h"

class CSceneManager;

/***********************************************
*	シーン基底クラス.
*		各シーンはこのクラスを継承して使用する.
**/
class CSceneBase
{
public:
	CSceneBase( CSceneManager* pSceneManager )
		: m_pSceneManager	( pSceneManager )
		, m_IsSceneChange	( false )
	{}

	virtual ~CSceneBase(){};
	
	// 読込関数.
	virtual bool Load() = 0;
	// 更新関数.
	virtual void Update() = 0;
	// モデル描画関数.
	virtual void ModelRender() = 0;
	// 画像描画関数.
	virtual void SpriteRender() = 0;

	// シーンの変更.
	void ChangeScene();
protected:
	// シーンの変更の設定.
	//	シーンを変更する前に呼ぶ.
	void SetSceneChange();


protected:
	CSceneManager*	m_pSceneManager;	// シーン情報.
	bool			m_IsSceneChange;	// シーンを変更するか.

private:
	// コピー・ムーブコンストラクタ, 代入演算子の削除.
	CSceneBase( const CSceneBase & )				= delete;
	CSceneBase& operator = ( const CSceneBase & )	= delete;
	CSceneBase( CSceneBase && )						= delete;
	CSceneBase& operator = ( CSceneBase && )		= delete;
};

#endif // #ifndef SCENE_BASE_H.