#ifndef TITLE_H
#define TITLE_H

#include "..\..\SceneBase\SceneBase.h"
#include "..\..\..\Common\Effect\EffectManager.h"
#include "..\..\..\Object\Collider\CollisionManager\CollisionManager.h"
#include "..\..\..\Object\CameraBase\RotLookAtCenter\RotLookAtCenter.h"
#include "..\..\..\Common\Mesh\RayMesh\RayMesh.h"

/**********************************
*	タイトルシーンクラス.
*/
class CTitle : public CSceneBase
{
public:
	CTitle( CSceneManager* pSceneManager );
	virtual ~CTitle();

	// 読込関数.
	virtual bool Load() override;
	// 更新関数.
	virtual void Update() override;
	// モデル描画関数.
	virtual void ModelRender() override;
	// 画像描画関数.
	virtual void SpriteRender() override;

private:
	float m_DeltaTime;
	CDX9SkinMesh*	m_pSkinMesh;
	CDX9StaticMesh*	m_pStaticMesh;
	CSprite*	m_Sprite;
	CEffectManager m_effect1;
	CEffectManager m_effect2;
};

#endif	// #ifndef TITLE_H.