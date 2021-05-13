#include "EffectResource.h"
#include "..\..\Common\Effect\Effect.h"

CEffectResource::CEffectResource()
	: m_EffectList			()
	, m_HasFinishedLoading	( false )
{
}

CEffectResource::~CEffectResource()
{
	Release();
}

//----------------------------.
// インスタンスの取得.
//----------------------------.
CEffectResource* CEffectResource::GetInstance()
{
	static std::unique_ptr<CEffectResource> pInstance =
		std::make_unique<CEffectResource>();
	return pInstance.get();
}

//----------------------------.
// 読み込み(ラッパー).
//----------------------------.
void CEffectResource::Load( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11 )
{
	// エフェクトの読み込み.
	GetInstance()->EffectLoad( pDevice11, pContext11 );
}

//----------------------------.
// 解放.
//----------------------------.
void CEffectResource::Release()
{
	GetInstance()->m_EffectList.clear();
}

//----------------------------.
// エフェクト取得関数.
//----------------------------.
CEffect* CEffectResource::GetEffect( const std::string& fileName )
{
	// 見つけたエフェクトを取得.
	for( auto& m : GetInstance()->m_EffectList ){
		if( m.first == fileName ) return m.second.get();
	}

	// 読込が終わっていなかったら null を返す.
	if( GetInstance()->m_HasFinishedLoading == false ) return nullptr;

	ERROR_MESSAGE( fileName + " effects not found" );
	return nullptr;
}

//----------------------------.
// エフェクトの読み込み.
//----------------------------.
void CEffectResource::EffectLoad(
	ID3D11Device* pDevice11,
	ID3D11DeviceContext* pContext11 )
{
	auto eachLoad = [&]( const fs::directory_entry& entry )
	{
		const std::string exe		= entry.path().extension().string();	// 拡張子.
		const std::string filePath	= entry.path().string();				// ファイルパス.
		const std::string fileName	= entry.path().stem().string();			// ファイル名.

		if( exe != ".efk" ) return;	// 拡張子が ".efk" じゃなければ終了.

		// エフェクトを読み込みリストに格納する.
		m_EffectList[fileName] = 
			std::make_unique<CEffect>( pDevice11, pContext11, filePath );
	};

	CLog::Print( "------ エフェクト読み込み開始 -------" );

	try {
		fs::recursive_directory_iterator dir_itr(FILE_PATH), end_itr;
		std::for_each( dir_itr, end_itr, eachLoad );

	} catch( const fs::filesystem_error& e ){
		// エラーメッセージを表示.
		ERROR_MESSAGE( e.path1().string().c_str() );
	}

	CLog::Print( "------ エフェクト読み込み終了 -------" );

	// 読込が終わったので true にする.
	m_HasFinishedLoading = true;
}