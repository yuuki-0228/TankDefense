#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <map>
#include <iostream>
#include <thread>
#include "XAudio2Play\XAudio2PlayBGM.h"
#include "XAudio2Play\XAudio2PlaySE.h"
#include "OggLoad/OggLoad.h"

/**
 * @file SoundManager.h
 * @brief XAudio2のサウンド再生機能をまとめたシングルトンクラス.
 * @author OTakotiri.
 */

class CSoundManager
{
public:
//=============================
// 構造体宣言.
//=============================
	struct SoundVolume
	{
		float MasterVolume = 1.0f;
		float BGMVolume = 1.0f;
		float SEVolume = 1.0f;
	};
public:
	CSoundManager();
	~CSoundManager();
	// インスタンスの取得.
	static CSoundManager* GetInstance();
	// サウンドデータ作成.
	static void CreateSoundData();
	// ゲーム全体のBGMの最大音量を取得.
	inline static const float GetGameBGMVolume()	{ return GetInstance()->m_stSound.BGMVolume * GetInstance()->m_stSound.MasterVolume; }
	// 最大マスター音量取得
	inline static const float GetMasterVolume()		{ return GetInstance()->m_stSound.MasterVolume; }
	// 最大BGM音量取得
	inline static const float GetBGMVolume()		{ return GetInstance()->m_stSound.BGMVolume; }
	// ゲーム全体のSEの最大音量を取得.
	inline static const float GetGameSEVolume()		{ return GetInstance()->m_stSound.SEVolume * GetInstance()->m_stSound.MasterVolume; }
	//========================================================================================
	//	BGM
	//====
	// スレッドを作ってBGMを再生する関数(外部呼出).
	static void ThreadPlayBGM(const std::string& Name, const bool& LoopFlag = true);
	// BGM停止関数.
	static void StopBGM(const std::string Name);
	// BGM一時停止関数.
	static void PauseBGM(const std::string Name);
	// BGM再再生関数
	static void AgainPlayBGM(const std::string Name);
	// BGMフェードアウト関数.
	static void FadeOutBGM(const std::string Name);
	// 指定BGMの音量のフェードアウト実行中かを返す.
	static const bool GetFadeOutBGM(const std::string Name);
	// BGMフェードイン関数.
	static void FadeInBGM(const std::string Name);
	// 指定BGMの音量のフェードイン実行中かを返す.
	static const bool GetFadeInBGM(const std::string Name);
	// BGMの音量取得関数.
	static const float GetBGMVolume(const std::string Name);
	// BGMの音量セット関数.
	static void SetBGMVolume(const std::string Name, const float Volume);
	// BGMを止めて、スレッドを放棄する関数.
	static const bool StopBGMThread(const std::string BGMName);
	// 指定した名前のBGMスレッドが再生中かどうかを返す : 再生中ならtrue.
	static const bool GetIsPlayBGM(const std::string BGMName);
	// BGMのピッチ設定.
	static void SetBGMPitch(const std::string Name, const float Value);
	//========================================================================================
	//	SE
	//====
	// SE再生関数.
	static void PlaySE(const std::string& Name);
	// 多重再生しないSE再生関数.
	static void NoMultipleSEPlay(const std::string& Name);
	// SE停止関数.
	static void StopSE(const std::string Name, const size_t ArrayNum);
	// 同じ音源の全てのSEソース停止関数.
	static void StopAllSE(const std::string Name);
	// SEのVoiceソース内最大音量を設定.
	static void SetSEVolume(const std::string Name, const float Volume);
	// 指定した名前のSEが再生中かどうかを返す : 再生中ならtrue.
	static bool GetIsPlaySE(const std::string Name, const size_t ArrayNum);
	// 全体SE音量とは別のソース毎の音量をセット.
	static void SetAnotherSEVolume(const std::string Name, const float& Volume);
	// ソース内で設定されている音量を適用するかどうか.
	static void SetUseAnotherSEVolumeFlag(const std::string Name, const bool& bFlag);
	// 解放処理関数.
	static void Release();
	//========================================================================================
	// オプション画面用全ソースのサウンド調整.
	// マスター音量セット.
	static void SetMasterVolume(float& MasterVolume);
	// BGM,SEの音量を変更するためのスレッドを立ち上げる.
	static void CreateChangeSoundVolumeThread();
	// BGM,SEの音量を変更するためのスレッドを解放する.
	static bool ReleaseChangeSoundVolumeThread();
	//===========================================================
	// オプション用関数.
	// 音量を変更できるスレッドのステート変更.
	static void StateChangeVolumeThread(const bool& bFlag);
	// オプション用スレッド動作済みかどうかのフラグ取得関数.
	inline static const bool GetCreateOptionThread() { return GetInstance()->m_isCreateThread; }
public:
	SoundVolume m_stSound;
private:
	// BGM再生関数.
	static void PlayBGM(const std::string Name, bool& isEnd);
	// 音量を変更するスレッドを立てる.
	static void SetChangeVolumeThread();
	// 音量初期設定関数.
	static void VolumeInit();
	// 存在などのチェック.
	static bool CheckBGMDataIsTrue(const std::string Name);
private:
	// Oggデータクラスの箱
	std::unordered_map <std::string, std::shared_ptr<COggLoad>>	m_pOggWavData;
	// BGM用PlaySoundクラスの箱.
	std::unordered_map <std::string, std::shared_ptr<CXAudio2PlayBGM>> pBgmSource;
	// BGMストリーミング再生用スレッド.
	std::unordered_map <std::string, std::thread> pBGMThread;
	// スレッド内で再生しているBGMを停止するためのフラグ.
	std::unordered_map <std::string, bool> m_bisEndThread;
	// スレッドリリース用関数.
	std::unordered_map <std::string, bool> m_bisThreadRelease;
	// 各スレッドID保持.
	std::unordered_map <std::string, std::thread::id> InThreadID;
	// BGMの名前リスト.
	// 解放時に必要.
	std::vector<std::string>	m_vsBGMNameList;
	// SEの名前リスト.
	// 解放時に必要.
	std::vector<std::string>	m_vsSENameList;
	// SE用PlaySoundクラスの箱.
	std::unordered_map <std::string, std::shared_ptr<CXAudio2PlaySE>> pSeSource;
	// 終了用フラグ.
	bool	m_bEndGame;
	// BGM,SEの作成が終わった後かどうか.
	bool	m_bEndCreate;
//=================================================
// 以下オプション画面用.
	// 以下二つはスレッドで回さないとストリーミングの入れ込みを待ってからの変更になるため
	// スレッドに回す.
	std::thread m_SoundSourceVolume;	// オプション画面で、音量を変更するためのスレッド.
	bool	m_bMoveSoundVolumeThread;	// オプション画面で、BGMを設定と同時に変更するためのスレッド用変数.
	bool	m_isCreateThread;	// 音量変更のスレッドが動いているかどうか.
//------------------------------------------
// スレッドロック用.
	bool	m_bResumeSoundVolumeThread;	// 条件変数.
	std::mutex m_SoundVolumemtx;
	std::condition_variable m_SoundVolumeCv;
private:
	// コピー・ムーブコンストラクタ, 代入演算子の削除.
	CSoundManager(const CSoundManager &) = delete;
	CSoundManager& operator = (const CSoundManager &) = delete;
	CSoundManager(CSoundManager &&) = delete;
	CSoundManager& operator = (CSoundManager &&) = delete;
};

#endif // #ifndef SOUNDMANAGER_H.
