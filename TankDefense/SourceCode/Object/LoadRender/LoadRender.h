/**
* @file LoadRender.h.
* @brief ロード時に描画させるクラス.
* @author 福田玲也.
*/
#ifndef LOAD_RENDER_H
#define LOAD_RENDER_H

#include "..\Object.h"

/****************************************
*	ロード画像描画クラス.
**/
class CLoadRender : public CObject
{
public:
	CLoadRender();
	virtual ~CLoadRender();

	// 読み込みクラス.
	bool Init();

	// 更新関数.
	void Update( const float& deltaTime );
	// 描画クラス.
	void Render();

private:
	// スプライトの読み込み.
	bool LoadSprite();

private:
	std::vector<std::unique_ptr<CSprite>>	m_pSprites;			// 画像クラス.
	float									m_DeltaTime;		// デルタタイム.
	bool									m_IsLoadEnd;		// 読み込みが終了したか.
};

#endif	// #ifndef LOAD_RENDER_H.