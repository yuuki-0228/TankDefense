#include "..\SceneList.h"
#include "..\..\..\Common\Mesh\Dx9SkinMesh\Dx9SkinMesh.h"
#include "..\..\..\Resource\MeshResource\MeshResource.h"
#include "..\..\..\Utility\Input\Input.h"
#include "..\..\..\Object\Fade\Fade.h"
#include "..\..\..\Object\CameraBase\CameraManager\CameraManager.h"

CGame::CGame( CSceneManager* pSceneManager )
	: CSceneBase			( pSceneManager )
{
}

CGame::~CGame()
{
}

//============================.
//	読込関数.
//============================.
bool CGame::Load()
{
	// ゲームシーン用のカメラを用意していないため.
	//	フリーカメラを有効化する.
	CCameraManager::SetActiveFreeCamera();
	return true;
}

//============================.
//	更新関数.
//============================.
void CGame::Update()
{
	if( CKeyInput::IsMomentPress('G') == true ){
		SetSceneChange();
	}
}

//============================.
// モデル描画関数.
//============================.
void CGame::ModelRender()
{
}

//============================.
// 画像描画関数.
//============================.
void CGame::SpriteRender()
{
}
