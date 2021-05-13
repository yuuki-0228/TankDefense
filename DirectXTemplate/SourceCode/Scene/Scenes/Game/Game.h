#ifndef GAME_H
#define GAME_H

#include "..\..\SceneBase\SceneBase.h"

/**********************************
*	ゲームシーンクラス.
*/
class CGame : public CSceneBase
{
private:

public:
	CGame( CSceneManager* pSceneManager );
	virtual ~CGame();

	// 読込関数.
	virtual bool Load() override;
	// 更新関数.
	virtual void Update() override;
	// モデル描画関数.
	virtual void ModelRender() override;
	// 画像描画関数.
	virtual void SpriteRender() override;

private:

private:
};

#endif	// #ifndef GAME_H.