# DirectXTemplate
DirectX用テンプレート

## 説明
モデルの読み込み・描画、画像の読み込み・描画などの処理を実装し、ゲーム部分を重点的に考えて作れるような、主に自分用のテンプレートプロジェクト。  

## 主な使用方法

- [ウィンドウ設定](#ウィンドウ設定)
- [シーンの追加方法](#シーンの追加)


### ウィンドウ設定  
ウィンドウ名、ウィンドウタイトルの変更.
<code>Main.cpp</code>内の25,26行目の"Test"を変更したい名前に変更する  
~~~C++ Main.cpp
namespace
{
	constexpr char	WND_TITLE[]	= "Test";	// ウィンドウ名.
	constexpr char	APP_NAME[]	= "Test";	// アプリ名.
...
~~~

ウィンドウサイズの変更.
<code>Global.h</code>内の30,31行目を変更.
~~~C++ Global.h
constexpr int WND_W	= 1280;	// ウィンドウの幅.
constexpr int WND_H	= 720;	// ウィンドウの高さ.
~~~
### シーンの追加  
<code>SourceCode/Scene/Scenes</code> に追加したいシーンのcpp,hを作成し  
以下のソースコードをコピー&ペーストする  

<details>

~~~C++ Scene.h {.copy}
#include "..\..\SceneBase\SceneBase.h"

class CScene : public CSceneBase
{
public:
	CScene( CSceneManager* pSceneManager );
	virtual ~CScene();

	// 読込関数.
	virtual bool Load() override;
	// 更新関数.
	virtual void Update() override;
	// モデル描画関数.
	virtual void ModelRender() override;
	// 画像描画関数.
	virtual void SpriteRender() override;
};
~~~
~~~C++ Scene.cpp {.copy}
CScene::CScene( CSceneManager* pSceneManager )
	: CSceneBase	( pSceneManager )
{
}

CScene::~CScene()
{
}

//============================.
//	読込関数.
//============================.
bool CScene::Load()
{
	return true;
}

//============================.
//	更新関数.
//============================.
void CScene::Update()
{}

//============================.
// モデル描画関数.
//============================.
void CScene::ModelRender()
{}

//============================.
// 画像描画関数.
//============================.
void CScene::SpriteRender()
{}
~~~
</details>

### GameObjectの追加
#### Actor
<code>SourceCode\Object\GameObject\Actor</code>以下の階層にそれぞれ追加したいオブジェクトクラスのクラスを作成する  
その際、<code>CActor</code>クラスを継承させる  

<details>

~~~C++ test.h
class CTestObj : public CActor
{
public:
	CTestObj();
	virtual ~CTestObj();
	// 初期化関数.
	virtual bool Init() override;
	// 更新関数.
	virtual void Update( const float& deltaTime ) override;
	// 描画関数.
	virtual void Render() override;
	// 当たり判定関数.
	virtual void Collision( CActor* pActor ) override;
private:
	// 当たり判定の初期化.
	virtual void InitCollision() override;
	// 当たり判定の座標や、半径などの更新.
	virtual void UpdateCollision() override;
};
~~~
~~~C++ test.cpp
CTestObj::CTestObj()
{}
CTestObj::~CTestObj()
{}

// 初期化関数.
bool CTestObj::Init()
{
	InitCollision();	// 当たり判定の初期化.
	return true;
}
// 更新関数.
void CTestObj::Update( const float& deltaTime )
{
	m_DeltaTime = deltaTime;
	
	UpdateCollision();	// 当たり判定の更新.
}
// 描画関数.
void CTestObj::Render()
{
}
// 当たり判定関数.
void CTestObj::Collision( CActor* pActor )
{
}

// 当たり判定の初期化.
void CTestObj::InitCollision()
{
	m_pCollisions->InitCollision( ECollNo::Sphere );
	m_pCollisions->GetCollision<CSphere>()->SetRadius( 10.0f );
	
}
// 当たり判定の座標や、半径などの更新.
void CTestObj::UpdateCollision()
{
	m_pCollisions->GetCollision<CSphere>()->SetPosition( m_Tranceform.Position );
}
~~~

</details>

#### Widget
<code>SourceCode\Object\GameObject\Widget</code>以下の階層にそれぞれ追加したいUIオブジェクトクラスのクラスを作成する  
その際、<code>CWidget</code>クラスを継承させる  
