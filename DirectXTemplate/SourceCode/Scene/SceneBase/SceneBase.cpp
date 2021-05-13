#include "SceneBase.h"

#include "..\SceneManager\SceneManager.h"

// シーンの変更の設定.
//	シーンを変更する前に呼ぶ.
void CSceneBase::SetSceneChange()
{
	m_IsSceneChange = true; CFade::SetFadeIn();
}

// シーンの変更.
//	各シーンの更新関数の最後くらいで呼ぶ.
void CSceneBase::ChangeScene()
{
	if( m_IsSceneChange == true && CFade::IsFadeEnd() ) m_pSceneManager->ChangeNextScene();
}