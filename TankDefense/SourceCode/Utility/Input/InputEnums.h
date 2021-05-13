#ifndef INPUT_ENUMS_H
#define INPUT_ENUMS_H

// キーバインド.
// ゲーム毎に変更して使用する.
enum class enKeyBind : unsigned char
{
	None,

	Decision,		// 決定.
	Cancel,			// キャンセル.
	Skip,			// スキップ.

	Attack,			// 攻撃.
	SpecialAbility,	// 特殊能力.
	Avoidance,		// 回避.
	STGAttack,		// シューティング時の攻撃.

	Up,			// 上.
	Down,		// 下.
	Right,		// 右.
	Left,		// 左.

	UpAxis,		// 上軸.
	RightAxis,	// 右軸.

	Start,		// Xbox : Start,	DS3 : Start.
	Back,		// Xbox : Back,		DS3 : Select.

} typedef EKeyBind;

enum class enAxisBind : unsigned char
{
	None,

	L_Forward,	// 左スティックの前後.
	L_Right,	// 左スティックの左右.

	R_Forward,	// 右スティックの前後.
	R_Right,	// 右スティックの左右.
} typedef EAxisBind;

// 入力状態.
enum enInputState : unsigned char
{
	EInputState_NotPress	= 1 << 0,	// 押していない.
	EInputState_MomentPress	= 1 << 1,	// 押した瞬間.
	EInputState_Hold		= 1 << 2,	// 長押し.
	EInputState_Release		= 1 << 3,	// 離した瞬間.
} typedef EInputState;

#endif	// #ifndef INPUT_ENUMS_H.