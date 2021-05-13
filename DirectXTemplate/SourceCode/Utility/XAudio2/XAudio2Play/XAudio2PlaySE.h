#ifndef XAUDIO2_PLAYSE_H
#define XAUDIO2_PLAYSE_H


#include <vector>
#include <mutex>
#include "..\XAudio2.h"
#include "..\XAudio2Master\XAudio2MasterVoice.h"
#include "..\OggLoad\OggLoad.h"
#include "..\XAudio2CallBack\VoiceCallBack.h"

struct IXAudio2SourceVoice;

/**
 * @file XAudio2PlaySE.h
 * @brief XAudio2でのSE再生を目的としたクラス.
 * @author OTakotiri.
 */

class CXAudio2PlaySE
{
private:
	static const int SE_VOICE_SIZE = 10;	// SEの予備の数.
public:
	// コンストラクタ.
	CXAudio2PlaySE();
	// デストラクタ.
	virtual ~CXAudio2PlaySE();
	// SEが再生中かを返す.
	const bool IsPlayingSE(const size_t ArrayNum);
	// 多重再生可能なサウンド再生.
	virtual bool Play( std::shared_ptr<COggLoad> pWavData);
	// 多重再生しないサウンド再生.
	bool NoMultiplePlay(std::shared_ptr<COggLoad> pWavData);
	// SEサウンド停止.
	bool SeStop(const size_t ArrayNum);
	// 作成してる予備分のSEも停止.
	bool AllSeStop();

	HRESULT CreateOggSound(std::shared_ptr<COggLoad> pOggData, const char* filename);
	//=======================================================================
	// 基本音量操作系.
	//====

	// SEのVoiceソース内最大音量を設定.
	void SetMaxSEVolume(const float& fMaxVolume) { m_fSEVolume = fMaxVolume; }
	// SEのVoiceソース内最大音量を取得.
	const float GetMaxSEVolume() { return m_fSEVolume; }
	// SEの音量を取得します.
	const float GetSEVolume(size_t Array);
	// SEの音量を設定します.
	bool SetSEVolume(float value, size_t Array);
	// SoundSourceを破壊する関数.
	void DestoroySource();
	// 全体SE音量とは別のソース毎の音量をセット.
	void SetAnotherSEVolume(const float& fVolume) { m_fAnotherVolume = fVolume; }
	// ソース内で設定されている音量を適用するかどうか.
	void SetUseAnotherSEVolume(const float& bFlag) { m_bisAnotherVolume = bFlag; }
protected:
	int		CheckPlaySeBuffer();
protected:
	COggLoad*				m_pOggData; // Oggデータロードクラス.

	IXAudio2SourceVoice*	m_pSourceVoice[SE_VOICE_SIZE];		// 同じSEを同時にならせるように配列にする.
	bool				m_bisAnotherVolume;
	float				m_fAnotherVolume;
	int					m_CheckNonePlaySeVoice;					// 再生していないSEを洗い出す用.
	int					m_RepeatPlaySe;							// 全部が再生中の時に次に巻き戻して再生する配列番号を返す.		

	float m_fSEVolume;		// SE音量.
};

#endif // #ifndef XAUDIO2_PLAYSE_H