#include "FileManager.h"

namespace fileManager
{
	//----------------------------------------------.
	// ファイルの読み込み.
	//----------------------------------------------.
	std::vector<std::string> TextLoading( const std::string& fileName, const bool& isCommentOut, char delimiter )
	{
		std::vector<std::string> readList;	// 読み込みリスト.

		// ファイルを開く.
		std::fstream fileStream( fileName );
		// ファイルが開けない場合.
		if( !fileStream.is_open() ){
			return readList;
		}

		std::string line;	// 1行分を読み込むための文字.

		// 一行づつ読み込み.
		while( std::getline( fileStream, line ) ){
			if( isCommentOut == true ){
				// 取得した文字に '//' があればやり直し.
				if( line.find("//") != std::string::npos ) continue;
			}

			std::string buff = "";				// 文字を一時的に格納する.
			std::istringstream stream( line );
			if( isCommentOut == false ){
				readList.emplace_back( line );
				continue;
			}

			// デフォルト ',' 区切りで文字を取得.
			while( std::getline( stream, buff, delimiter ) ){
				if( isCommentOut == true ){
					// 取得した文字に '#' があればやり直し.
					if( buff.find('#') != std::string::npos ) continue;
				}

				// 読み込みリストに追加.
				readList.emplace_back( buff );
			}
		}

		fileStream.close();

		return readList;
	}
}
