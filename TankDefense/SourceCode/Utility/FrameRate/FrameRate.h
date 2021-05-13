/**
* @file FrameRate.h.
* @brief ゲームループのフレームレートを計算するクラス.
* @author 福田玲也.
*/
#ifndef FRAME_RATE_H
#define FRAME_RATE_H

#include <Windows.h>

/********************************************
*	フレームレートの計測クラス.
**/
class CFrameRate
{
public:
	CFrameRate();
	CFrameRate( const double& fps = 60.0 );
	~CFrameRate();

	// 待機関数.
	//	true の場合そのフレームを終了する.
	bool Wait();

	// FPSの取得.
	inline double GetFPS() const { return m_FPS; }
	// デルタタイムの取得.
	static inline double GetDeltaTime(){ return m_FrameTime; }

private:
	const double	FRAME_RATE;		// フレームレート.
	const double	MIN_FRAME_TIME;	// 最小フレームタイム.

private:
	static double	m_FrameTime;	// フレームタイム(デルタタイム).
	double			m_FPS;			// 現在のFPS.
	LARGE_INTEGER	m_StartTime;	// 開始時間.
	LARGE_INTEGER	m_NowTime;		// 現在の時間.
	LARGE_INTEGER	m_FreqTime;		// 起動した時間.
};

// デルタタイムの取得.
template<class T = float>
static inline T GetDeltaTime()
{
	return static_cast<T>(CFrameRate::GetDeltaTime());
}

#endif	// #ifndef FRAME_RATE_H.