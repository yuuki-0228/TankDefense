#include "SoundManager.h"
#include <crtdbg.h>
#include <filesystem>	// C++17 必須.

namespace fs = std::filesystem;
//=============================
// 定数宣言.
//=============================
namespace {
	constexpr char FILE_PATH[128] = "Data\\Sound";					// Soundデータが入っているディレクトリパス.
	constexpr char BINARY_FILE_PATH[128] = "Data\\Sound\\Data.bin";	// バイナリデータが入ってるパス.
	constexpr CSoundManager::SoundVolume INIT_VOLUME = { 0.5f,0.5f,0.5f };	// バイナリデータが存在しない際に設定する初期音量.
}

CSoundManager::CSoundManager()
	: m_bEndGame					( false )
	, m_bEndCreate					( false )
	, m_bMoveSoundVolumeThread		( true )
	, m_isCreateThread				( false )
	, m_bResumeSoundVolumeThread	( true )
{
	//---------------------------
	// Mapコンテナのクリア.
	pBgmSource.clear();
	pBGMThread.clear();
	m_bisEndThread.clear();
	m_bisThreadRelease.clear();
	InThreadID.clear();
	m_vsBGMNameList.clear();
	m_vsSENameList.clear();
	pSeSource.clear();
}

CSoundManager::~CSoundManager()
{
}
//=============================
// インスタンスの作成.
//=============================
CSoundManager* CSoundManager::GetInstance()
{
	static std::unique_ptr<CSoundManager> pInstance =
		std::make_unique<CSoundManager>();	// インスタンスの作成.
	return pInstance.get();
}
//=============================
// サウンドデータ作成.
//=============================
void CSoundManager::CreateSoundData()
{
	std::mutex	m_Mutex;
	m_Mutex.lock();
	if (GetInstance()->m_bEndCreate == true) return;
	auto eachLoad = [&](const fs::directory_entry& entry)
	{
		const std::string extension = entry.path().extension().string();	// 拡張子.
		const std::string filePath = entry.path().string();					// ファイルパス.
		const std::string fileName = entry.path().stem().string();			// ファイル名.
		// 拡張子がOggでなければ終了.
		if (extension != ".ogg" && extension != ".OGG") return;

		//ファイルパス内にBGMがなければSEの作成をする.
		if (filePath.find("BGM") == std::string::npos) {
			// WavLoadクラス作成.
			GetInstance()->m_pOggWavData[fileName] = std::make_shared<COggLoad>();
			// Wavフォーマット取得.
			GetInstance()->m_pOggWavData[fileName]->CreateOggData(fileName.c_str(), true);
			// PlaySoundクラス作成.
			GetInstance()->pSeSource[fileName] = std::make_shared<CXAudio2PlaySE>();
			// SEのSoundSource作成.
			GetInstance()->pSeSource[fileName]->CreateOggSound(GetInstance()->m_pOggWavData[fileName], GetInstance()->m_pOggWavData[fileName]->GetFileName());
			// フォルダの名前をSE名前リストに入れる.
			GetInstance()->m_vsSENameList.emplace_back(fileName);
		}
		else {
			// WavLoadクラス作成.
			GetInstance()->m_pOggWavData[fileName] = std::make_shared<COggLoad>();
			// Wavフォーマット取得.
			GetInstance()->m_pOggWavData[fileName]->CreateOggData(fileName.c_str());
			// PlaySoundクラス作成.
			GetInstance()->pBgmSource[fileName] = std::make_shared<CXAudio2PlayBGM>();
			// SEのSoundSource作成.
			GetInstance()->pBgmSource[fileName]->CreateOggSound(GetInstance()->m_pOggWavData[fileName], GetInstance()->m_pOggWavData[fileName]->GetFileName());
			// フォルダの名前をSE名前リストに入れる.
			GetInstance()->m_vsBGMNameList.emplace_back(fileName);
		}
	};

	try {
		//指定パス(FILE_PATH)ディレクトリ内を再帰的に走査
		fs::recursive_directory_iterator dir_itr(FILE_PATH), end_itr;
		// eachLoad関数を行う.
		std::for_each(dir_itr, end_itr, eachLoad);
	}
	// エラーキャッチ.
	catch (const fs::filesystem_error& e) {
		const char* errorMessage = e.path1().string().c_str();
		_ASSERT_EXPR(false, L"サウンドデータ作成失敗");
		MessageBox(nullptr, errorMessage, "サウンドデータ作成失敗", MB_OK);
	}
	// 音量設定.
	VolumeInit();
	// サウンド作成終了.
	GetInstance()->m_bEndCreate = true;
	m_Mutex.unlock();
}
//================================================
// BGM.
//=============================

//=============================
// スレッドを作ってBGMを再生する関数(外部呼出).
//=============================
void CSoundManager::ThreadPlayBGM(const std::string& Name, const bool& LoopFlag)
{
	if (Name.size() == 0) return;
	// データチェック.
	if (CheckBGMDataIsTrue(Name) == false) return;
	GetInstance()->m_bisEndThread[Name] = false;
	// スレッドに入れるラムダ関数.
	auto BGM = [&]()
	{
		CSoundManager::PlayBGM(Name, GetInstance()->m_bisEndThread[Name]);	// 再生関数.
	};

	DWORD ThreadExitCode = -1;
	// 指定したBGMの名前のスレッドからハンドルID取得.
	GetExitCodeThread(GetInstance()->pBGMThread[Name].native_handle(), &ThreadExitCode);
	// 0xFFFFFFFFが帰ってきたらスレッドが動いていないので動かす.
	if (ThreadExitCode == 0xFFFFFFFF) GetInstance()->pBGMThread[Name] = std::thread(BGM);
	// スレッドのハンドルIDを保持.
	GetInstance()->InThreadID[Name] = GetInstance()->pBGMThread[Name].get_id();
	// 鳴らしたBGMのスレッドの解放フラグを下す.
	GetInstance()->m_bisThreadRelease[Name] = false;
}
//=============================
// BGM再生関数(内部使用).
//=============================
void CSoundManager::PlayBGM(const std::string Name, bool& isEnd)
{
	// データチェック.
	if (CheckBGMDataIsTrue(Name) == false) return;
	// 再生.
	GetInstance()->pBgmSource[Name]->Play(GetInstance()->m_pOggWavData[Name],
		GetInstance()->m_pOggWavData[Name]->GetFileName(), isEnd);
}
//=============================
// BGM一時停止.
//=============================
void CSoundManager::PauseBGM(const std::string Name)
{
	// mapコンテナにキーが存在してなければリターン.
	if (GetInstance()->pBgmSource.find(Name) == GetInstance()->pBgmSource.end()) {
		return;
	}

	// 存在しなければリターン.
	if (GetInstance()->pBgmSource[Name] == nullptr) return;
	// ゲーム終了フラグがたっていればリターン.
	if (GetInstance()->m_bEndGame == true) return;
	// 指定したBGM停止.
	GetInstance()->pBgmSource[Name]->Pause();
}
//=============================
// BGM再開関数.
//=============================
void CSoundManager::AgainPlayBGM(const std::string Name)
{
	// mapコンテナにキーが存在してなければリターン.
	if (GetInstance()->pBgmSource.find(Name) == GetInstance()->pBgmSource.end()) {
		return;
	}

	// 存在しなければリターン.
	if (GetInstance()->pBgmSource[Name] == nullptr) return;
	// ゲーム終了フラグがたっていればリターン.
	if (GetInstance()->m_bEndGame == true) return;
	// 指定したBGM停止.
	GetInstance()->pBgmSource[Name]->PlayStart();
}

//=============================
// 音量を変更できるスレッドのステート変更.
//=============================
void CSoundManager::StateChangeVolumeThread(const bool& bFlag)
{
	GetInstance()->m_bResumeSoundVolumeThread = bFlag;
	// 入れたフラグがtrueの時、元がfalseの場合、スレッドがサスペンドしているため.
	// 音量を変更するスレッドを起床させる.
	if (bFlag == true) GetInstance()->m_SoundVolumeCv.notify_one();
}
//=============================
// BGM停止関数.
//=============================
void CSoundManager::StopBGM(const std::string Name)
{
	// データチェック.
	if (CheckBGMDataIsTrue(Name) == false) return;
	// 指定したBGM停止.
	GetInstance()->pBgmSource[Name]->Stop();
}
//=============================
// BGMフェードアウト関数.
//=============================
void CSoundManager::FadeOutBGM(const std::string Name)
{
	if (CheckBGMDataIsTrue(Name) == false) return;			// データチェック.
	GetInstance()->pBgmSource[Name]->SetFadeInFlag(false);	// フェードイン中だった場合、それを止めフェードアウトを始める.
	// フェードアウトフラグを立てる.
	if (GetInstance()->pBgmSource[Name]->GetFadeOutFlag() == false) GetInstance()->pBgmSource[Name]->SetFadeOutFlag(true);
}
//=============================
// 指定BGMの音量のフェードアウト実行中かを返す.
//=============================
const bool CSoundManager::GetFadeOutBGM(const std::string Name)
{
	if (CheckBGMDataIsTrue(Name) == false) return false; // データチェック.
	return GetInstance()->pBgmSource[Name]->GetFadeOutFlag();
}
//=============================
// BGMフェードイン関数.
//=============================
void CSoundManager::FadeInBGM(const std::string Name)
{
	if (CheckBGMDataIsTrue(Name) == false) return;			// データチェック.	
	GetInstance()->pBgmSource[Name]->SetFadeOutFlag(false); // フェードアウト中だった場合、それを止めフェードインを始める.
	// フェードインフラグを立てる.
	if (GetInstance()->pBgmSource[Name]->GetFadeInFlag() == false ) GetInstance()->pBgmSource[Name]->SetFadeInFlag(true);
}
//=============================
// 指定BGMの音量のフェードイン実行中かを返す.
//=============================
const bool CSoundManager::GetFadeInBGM(const std::string Name)
{
	if (CheckBGMDataIsTrue(Name) == false) return false; // データチェック.
	return GetInstance()->pBgmSource[Name]->GetFadeInFlag();
}
//=============================
// 指定したBGMの音量を取得する関数.
//=============================
const float CSoundManager::GetBGMVolume(const std::string Name)
{
	// データチェック.
	if (CheckBGMDataIsTrue(Name) == false) return 0.0f;
	// 再生していない.
	if (GetInstance()->pBgmSource[Name]->IsPlaying() == false) return 0.0f;
	return GetInstance()->pBgmSource[Name]->GetVolume();
}
//=============================
// 指定したBGMの音量をセットする関数.
//=============================
void CSoundManager::SetBGMVolume(const std::string Name, float Volme)
{
	// ゲーム終了フラグがたっていればリターン.
	if (GetInstance()->m_bEndGame == true) return;

	GetInstance()->pBgmSource[Name]->SetBGMVolume(Volme);
}
//=============================
// BGMを停止し、そのBGMを再生していたスレッドを放棄する.
//=============================
const bool CSoundManager::StopBGMThread(const std::string BGMName)
{
	// スレッドが解放されていればリターン.
	if (GetInstance()->m_bisThreadRelease[BGMName] == true) return true;
	// 再生時に設定したフラグをtrueにして、BGMを停止に向かわせる.
	GetInstance()->m_bisEndThread[BGMName] = true;
	DWORD ThreadExitCode = -1;
	// スレッドが停止したかどうかをID取得で取得.
	GetExitCodeThread(GetInstance()->pBGMThread[BGMName].native_handle(), &ThreadExitCode);
	// 返って来る値が4294967295の場合、そもそもスレッドが立ち上がっていない(再生していない)のでリターン.
	if (ThreadExitCode == 0xFFFFFFFF) return true;
	// 停止していたら0が返って来る.
	if (ThreadExitCode == 0) {
		// スレッドから帰ってくるIDが、再生時に保持したIDと同じかをチェック.
		if (GetInstance()->InThreadID[BGMName] == GetInstance()->pBGMThread[BGMName].get_id()) {
			GetInstance()->pBGMThread[BGMName].detach();
			GetInstance()->m_bisThreadRelease[BGMName] = true;
			return true;
		}
		return false;
	}
	return false;
}
//=============================
// 指定した名前のBGMスレッドが再生中かどうか.
//=============================
const bool CSoundManager::GetIsPlayBGM(const std::string BGMName)
{
	DWORD ThreadExitCode = -1;
	// スレッドが停止したかどうかをID取得で取得.
	GetExitCodeThread(GetInstance()->pBGMThread[BGMName].native_handle(), &ThreadExitCode);
	// 返って来る値が4294967295の場合、再生していない.
	if (ThreadExitCode == 0xFFFFFFFF) return false;
	// 0ならスレッド停止済み.
	else if (ThreadExitCode == 0) return false;
	return true;
}
//=============================
// BGMのピッチ設定.
//=============================
void CSoundManager::SetBGMPitch(const std::string Name, const float Value)
{
	// 存在しなければリターン.
	if (GetInstance()->pBgmSource[Name] == nullptr) return;
	GetInstance()->pBgmSource[Name]->SetPitch(Value);
}

//========================================================================================
//	SE.
//====

//=============================
// SE再生関数.
//=============================
void CSoundManager::PlaySE( const std::string& Name)
{
	// mapコンテナにキーが存在してなければリターン.
	if (GetInstance()->pSeSource.find(Name) == GetInstance()->pSeSource.end()) return;
	if (GetInstance()->pSeSource[Name] == nullptr) return;
	// 再生.
	GetInstance()->pSeSource[Name]->Play(GetInstance()->m_pOggWavData[Name]);
}
//=============================
// 多重再生しないSE再生関数.
//=============================
void CSoundManager::NoMultipleSEPlay(const std::string & Name)
{
	// mapコンテナにキーが存在してなければリターン.
	if (GetInstance()->pSeSource.find(Name) == GetInstance()->pSeSource.end()) return;
	if (GetInstance()->pSeSource[Name] == nullptr)  return;
	// 再生.
	GetInstance()->pSeSource[Name]->NoMultiplePlay(GetInstance()->m_pOggWavData[Name]);
}
//=============================
// SE停止関数.
//=============================
void CSoundManager::StopSE(const std::string Name, const size_t ArrayNum)
{
	// 存在しなければリターン.
	if (GetInstance()->pSeSource[Name] == nullptr) return;
	GetInstance()->pSeSource[Name]->SeStop(ArrayNum);
}
//=============================
// 同じ音源の全てのSEソース停止関数.
//=============================
void CSoundManager::StopAllSE(const std::string Name)
{
	// 存在しなければリターン.
	if (GetInstance()->pSeSource[Name] == nullptr) return;
	GetInstance()->pSeSource[Name]->AllSeStop();
}
//=============================
// SEのVoiceソース内最大音量を設定.
//=============================
void CSoundManager::SetSEVolume(const std::string Name, const float Volume)
{
	// 存在しなければリターン.
	if (GetInstance()->pSeSource[Name] == nullptr) return;
	GetInstance()->pSeSource[Name]->SetMaxSEVolume(Volume);
}
//=============================
// 指定した名前のSEが再生中かどうかを返す : 再生中ならtrue.
//=============================
bool CSoundManager::GetIsPlaySE(const std::string Name, const size_t ArrayNum)
{
	// 存在しなければリターンfalse.
	if (GetInstance()->pSeSource[Name] == nullptr) return false;
	return GetInstance()->pSeSource[Name]->IsPlayingSE(ArrayNum);
}
//=============================
// 全体SE音量とは別のサウンドソース毎の音量をセット.
//=============================
void CSoundManager::SetAnotherSEVolume(const std::string Name, const float & Volume)
{
	// 存在しなければリターン.
	if (GetInstance()->pSeSource[Name] == nullptr) return;
	GetInstance()->pSeSource[Name]->SetAnotherSEVolume(Volume);
}
//=============================
// ソース内で設定されている音量を適用するかどうか.
//=============================
void CSoundManager::SetUseAnotherSEVolumeFlag(const std::string Name, const bool & bFlag)
{
	// 存在しなければリターン.
	if (GetInstance()->pSeSource[Name] == nullptr) return;
	GetInstance()->pSeSource[Name]->SetUseAnotherSEVolume(bFlag);
}
//=============================
// 解放関数.
//=============================
void CSoundManager::Release()
{
	// 音量を変更するスレッドの解放待ち.
	while (ReleaseChangeSoundVolumeThread() == false);
	// 音量をバイナリデータにセーブ.
	XAudio2File::CreateBinary(BINARY_FILE_PATH, GetInstance()->m_stSound);

	// SEの名前リスト数分ループを回す.
	for (size_t i = 0; i < GetInstance()->m_vsSENameList.size(); i++) {
		StopAllSE(GetInstance()->m_vsSENameList[i]);
		GetInstance()->m_pOggWavData[GetInstance()->m_vsSENameList[i]]->Close();			//WavDataを閉じる.
		GetInstance()->pSeSource[GetInstance()->m_vsSENameList[i]]->DestoroySource();	// SoundSourceを解放.
	}
	// BGMの名前リスト数分ループを回す.
	for (size_t i = 0; i < GetInstance()->m_vsBGMNameList.size(); i++) {
		while (StopBGMThread(GetInstance()->m_vsBGMNameList[i]) == false);
		GetInstance()->m_pOggWavData[GetInstance()->m_vsBGMNameList[i]]->Close();		//WavDataを閉じる.
		GetInstance()->pBgmSource[GetInstance()->m_vsBGMNameList[i]]->DestoroySource();	// SoundSourceを解放.
	}
	// ゲーム終了フラグを立てる.
	GetInstance()->m_bEndGame = true;
}
//=============================
// マスター音量セット.
//=============================
void CSoundManager::SetMasterVolume(float& MasterVolume)
{
	if (MasterVolume >= 1.0f) MasterVolume = 1.0f;
	if (MasterVolume <= 0.0f) MasterVolume = 0.0f;
	GetInstance()->m_stSound.MasterVolume = MasterVolume;
}
//=============================
// BGM,SEの音量を変更するためのスレッドを立ち上げる.
//=============================
void CSoundManager::CreateChangeSoundVolumeThread()
{
	// スレッド作成のフラグが立っていればリターン.
	if (GetInstance()->m_isCreateThread == true) return;
	SetChangeVolumeThread();	// スレッド作成.
	GetInstance()->m_isCreateThread = true;
}
//=============================
// 音量を変更するスレッドを立てる.
//=============================
void CSoundManager::SetChangeVolumeThread()
{
	DWORD ThreadExitCode = -1;
	// スレッド状態を取得.
	GetExitCodeThread(GetInstance()->m_SoundSourceVolume.native_handle(), &ThreadExitCode);
	// スレッドが動いていればリターン.
	if (ThreadExitCode != 0xFFFFFFFF && ThreadExitCode != 0) {
		return;
	}
	GetInstance()->m_bMoveSoundVolumeThread = true;
	auto SetBGMVolme = [&]()
	{
		while (GetInstance()->m_bMoveSoundVolumeThread)
		{
			std::unique_lock<std::mutex> lk(GetInstance()->m_SoundVolumemtx); // mutex.
			// スレッドをm_bMoveSoundVolumeThreadがfalseの間ここでサスペンド(一切動かさない)、trueで再開.
			GetInstance()->m_SoundVolumeCv.wait(lk, [&] { return GetInstance()->m_bResumeSoundVolumeThread; });
			// BGM用ループ.
			for (size_t i = 0; i < GetInstance()->m_vsBGMNameList.size(); i++) {
				GetInstance()->pBgmSource[GetInstance()->m_vsBGMNameList[i]]->SetBGMVolume(CSoundManager::GetInstance()->m_stSound.BGMVolume);	// 音量をセット.
				if (GetInstance()->m_bMoveSoundVolumeThread == false) break;
			}
			// SE用ループ.
			for (size_t i = 0; i < GetInstance()->m_vsSENameList.size(); i++) {
				GetInstance()->pSeSource[GetInstance()->m_vsSENameList[i]]->SetMaxSEVolume(CSoundManager::GetInstance()->m_stSound.SEVolume);	// 音量をセット.
				if (GetInstance()->m_bMoveSoundVolumeThread == false) break;
			}
		}
	};
	GetInstance()->m_SoundSourceVolume = std::thread(SetBGMVolme);
}
//=============================
// 音量初期設定関数.
//=============================
void CSoundManager::VolumeInit()
{
	HRESULT hr;
	// バイナリデータから音量を読み込む.
	hr = XAudio2File::LoadBinary(BINARY_FILE_PATH, GetInstance()->m_stSound);
	// バイナリデータが存在しない場合.
	if (hr == E_FAIL) {
		// バイナリデータ新規作成.
		XAudio2File::CreateBinary(BINARY_FILE_PATH, INIT_VOLUME);
		GetInstance()->m_stSound = INIT_VOLUME;
	}
	// BGM用ループ.
	for (size_t i = 0; i < GetInstance()->m_vsBGMNameList.size(); i++) {
		GetInstance()->pBgmSource[GetInstance()->m_vsBGMNameList[i]]->SetBGMVolume(CSoundManager::GetInstance()->m_stSound.BGMVolume);	// 音量をセット.
		if (GetInstance()->m_bMoveSoundVolumeThread == false) break;
	}
	// SE用ループ.
	for (size_t i = 0; i < GetInstance()->m_vsSENameList.size(); i++) {
		GetInstance()->pSeSource[GetInstance()->m_vsSENameList[i]]->SetMaxSEVolume(CSoundManager::GetInstance()->m_stSound.SEVolume);	// 音量をセット.
		if (GetInstance()->m_bMoveSoundVolumeThread == false) break;
	}
	SetMasterVolume(GetInstance()->m_stSound.MasterVolume);
}
bool CSoundManager::CheckBGMDataIsTrue(const std::string Name)
{
	// mapコンテナにキーが存在しているか.
	if (GetInstance()->pBgmSource.find(Name) == GetInstance()->pBgmSource.end()) return false;
	// 中身があるか.
	if (GetInstance()->pBgmSource[Name] == nullptr) return false;
	// ゲーム終了フラグがたっていないか.
	if (GetInstance()->m_bEndGame == true) return false;
	return true;
}
//=============================
// BGM,SEの音量を変更するためのスレッドを解放する.
//=============================
bool CSoundManager::ReleaseChangeSoundVolumeThread()
{
	GetInstance()->m_bResumeSoundVolumeThread = true;
	// スレッドを起床させる.
	GetInstance()->m_SoundVolumeCv.notify_one();
	// スレッドが解放できる状態(スレッドが動いている). 
	if (GetInstance()->m_SoundSourceVolume.joinable() == true) {
		GetInstance()->m_bMoveSoundVolumeThread = false;
		GetInstance()->m_SoundSourceVolume.join();
		while (1)
		{
			if (GetInstance()->m_SoundSourceVolume.joinable() != true) {
				break;
			}
		}
	}
	GetInstance()->m_isCreateThread = false;
	return true;
}
