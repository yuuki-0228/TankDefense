#ifndef SPRITE_STRUCT_H
#define SPRITE_STRUCT_H

#include "..\..\Global.h"

// 頂点の構造体.
struct SpriteVertex
{
	D3DXVECTOR3 Pos;	// 頂点座標.
	D3DXVECTOR2	Tex;	// テクスチャ座標.
};

// 幅高さ構造体(float型).
struct WHDIZE_FLOAT
{
	float w;	// 幅.
	float h;	// 高さ.
};

// ローカル座標の番号.
enum class enLocalPosition : unsigned char
{
	None,

	LeftUp = 0,	// 左上.
	Left,		// 左.
	LeftDown,	// 左下.
	Down,		// 下.
	RightDown,	// 右下.
	Right,		// 右.
	RightUp,	// 右上.
	Up,			// 上.
	Center,		// 中央.

	Max,
} typedef ELocalPosition;

// サンプラステート.
enum enSamplerState : unsigned char
{
	ESamplerState_None,

	ESamplerState_Wrap = 0,	// タイル状に繰り返される.
	ESamplerState_Mirror,	// 反転しながら繰り返される.
	ESamplerState_Clamp,	// 0~1に設定される.
	ESamplerState_Border,	// 0~1の範囲外はhlslで指定した色になる.
	ESamplerState_MirrorOne,// 0を中心に反転される.

	ESamplerState_Max,

} typedef ESamplerState;

enum enSpriteStateNo : unsigned char
{
	None,

	ESpriteStateNo_LocalPosNum = 0,
	ESpriteStateNo_Disp_w,
	ESpriteStateNo_Disp_h,
	ESpriteStateNo_Base_w,
	ESpriteStateNo_Base_h,
	ESpriteStateNo_Stride_w,
	ESpriteStateNo_Stride_h,
	ESpriteStateNo_vPos_x,
	ESpriteStateNo_vPos_y,
	ESpriteStateNo_vPos_z,
	ESpriteStateNo_AnimNum,

	ESpriteStateNo_Max = ESpriteStateNo_AnimNum,
} typedef ESpriteStateNo;

enum enPSShaderNo : unsigned char
{
	EPSShaderNo_None,

	EPSShaderNo_Main = 0,
	EPSShaderNo_TAlphaOut,
	EPSShaderNo_TCutOut,

	EPSShaderNo_Max,
} typedef EPSShaderNo;

// スプライト構造体.
struct stSpriteState
{
	ELocalPosition	LocalPosNum;// ローカル座標の番号.
	WHDIZE_FLOAT	Disp;		// 表示幅,高さ.
	WHDIZE_FLOAT	Base;		// 元画像の幅,高さ.
	WHDIZE_FLOAT	Stride;		// 1コマ当たりの幅,高さ.
	D3DXVECTOR3		vPos;		// 座標.
	int				AnimNum;	// アニメーション数.
} typedef SSpriteState;

// スプライトアニメーション情報.
struct stSpriteAnimState
{
	POINT		PatternNo;			// パターン番号.
	POINT		PatternMax;			// パターン最大数.
	D3DXVECTOR2	UV;					// テクスチャUV座標.
	D3DXVECTOR2	ScrollSpeed;		// テクスチャUV座標.
	int			AnimNumber;			// アニメーション番号.
	int			MaxAnimNumber;		// 最大アニメーション番号.
	int			FrameCount;			// フレームカウント.
	int			FrameCountSpeed;	// フレームカウント速度.
	int			FrameTime;			// フレームタイム.
	bool		IsAnimation;		// アニメーションするか.
	bool		IsUVScrollX;		// UVスクロールをするか.
	bool		IsUVScrollY;		// UVスクロールをするか.


	stSpriteAnimState()
		: PatternNo			{ 1, 1 }
		, PatternMax		{ 1, 1 }
		, UV				( 0.0f, 0.0f )
		, ScrollSpeed		( 0.001f, 0.001f )
		, AnimNumber		( 1 )
		, MaxAnimNumber		( 1 )
		, FrameCount		( 0 )
		, FrameCountSpeed	( 1 )
		, FrameTime			( 5 )
		, IsAnimation		( false )
		, IsUVScrollX		( false )
		, IsUVScrollY		( false )
	{}

	// UV座標に変換.
	D3DXVECTOR2 ConvertIntoUV()
	{
		return {
			// x座標.
			static_cast<float>(PatternNo.x) / static_cast<float>(PatternMax.x),
			// y座標.
			static_cast<float>(PatternNo.y) / static_cast<float>(PatternMax.y)
		};
	}

	// アニメーションの更新.
	void AnimUpdate()
	{
		if( IsAnimation == false ) return;

		if( FrameCount % FrameTime == 0 ){
			// アニメーション番号を算出.
			PatternNo.x = AnimNumber % PatternMax.x;
			PatternNo.y = AnimNumber / PatternMax.x;
			AnimNumber++;	// アニメーション番号を加算.
			if( AnimNumber >= MaxAnimNumber ){
				// アニメーション番号が最大アニメーション数より多ければ.
				// 初期化する.
				FrameCount = 0;
				AnimNumber = 0;
			}
		}
		FrameCount += FrameCountSpeed;
		// UV座標に変換.
		UV = ConvertIntoUV();
	}

	// UVスクロールの更新.
	void UVScrollUpdate()
	{
		if( IsUVScrollX == true ){
			UV.x += ScrollSpeed.x;
			if( UV.x > 1.0f ) UV.x = 0.0f;
			if( UV.x < 0.0f ) UV.x = 1.0f;
		}

		if( IsUVScrollY == true ){
			UV.y += ScrollSpeed.y;
			if( UV.y > 1.0f ) UV.y = 0.0f;
			if( UV.y < 0.0f ) UV.y = 1.0f;
		}
	}

} typedef SSpriteAnimState;

// 描画用画像情報.
struct stSpriteRenderState
{
	SSpriteAnimState	AnimState;	// アニメーション情報.
	STranceform			Tranceform;	// 位置情報など.
	D3DXVECTOR4			Color;		// 色.
	float				FadeValue;	// フェード、トランジションの値.
	EPSShaderNo			ShaderNo;	// シェーダーの番号.
	ESamplerState		SmaplerNo;	// サンプラ番号.
	bool				IsUIRender;	// UIで描画するか false なら3Dで描画する.

	stSpriteRenderState()
		: AnimState		()
		, Tranceform	()
		, Color			( 1.0f, 1.0f, 1.0f, 1.0f )
		, FadeValue		( 0.0f )
		, ShaderNo		( EPSShaderNo_Main )
		, SmaplerNo		( ESamplerState_Wrap )
		, IsUIRender	( true )
	{}

} typedef SSpriteRenderState;

#endif	// #ifndef SPRITE_STRUCT_H.