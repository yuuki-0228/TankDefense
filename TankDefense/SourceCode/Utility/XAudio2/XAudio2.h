#ifndef XAUDIO2_H
#define XAUDIO2_H

#include <xaudio2.h>
#include <xaudio2fx.h>
#include <fstream>

#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"xaudio2.lib")

//--------------------------
// セーフ開放マクロ.

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p)=nullptr; } }
#endif  
#ifndef SAVE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p); (p)=nullptr; } }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=nullptr; } }
#endif

namespace XAudio2File
{
	// バイナリデータを読み込む.
	template <class In>
	static HRESULT LoadBinary(const std::string Path, In& InData)
	{
		std::ifstream fin(Path.c_str(), std::ios::in | std::ios::binary);

		if (!fin) {
			return E_FAIL;
		}
		// ファイルを読み込む.
		fin.read(reinterpret_cast<char*>(std::addressof(InData)), sizeof(In));

		fin.close();  //ファイルを閉じる
		return S_OK;
	};
	// バイナリデータを作成する.
	template <class Out>
	static HRESULT CreateBinary(const std::string Path, Out OutData)
	{
		std::ofstream fout;

		fout.open(Path.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);

		if (!fout) {
			return E_FAIL;
		}

		fout.write(reinterpret_cast<char*>(std::addressof(OutData)), sizeof(Out));
		fout.close();  //ファイルを閉じる

		return S_OK;
	};
}



#endif//#ifndef XAUDIO2_H.