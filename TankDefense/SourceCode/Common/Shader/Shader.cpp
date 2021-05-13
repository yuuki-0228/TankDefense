#include "Shader.h"

namespace shader
{

//----------------------------------.
// シェーダー読込関数.
//----------------------------------.
HRESULT InitShader(
	const char* filePath,
	const char* entryPoint,
	const char* shaderModel,
	UINT compileFlag,
	ID3DBlob** ppCompiledShader,
	ID3DBlob** ppErrors )
{
	return D3DX11CompileFromFile(
		filePath,			// シェーダーファイルパス.
		nullptr,			// マクロ定義のポインター : オプション.
		nullptr,			// インクルードファイルを処理するためのポインタ : オプション.
		entryPoint,			// シェーダーのエントリーポイント関数名.
		shaderModel,		// シェーダーモデルの名前.
		compileFlag,		// シェーダーコンパイルフラグ.
		0,					// エフェクトコンパイルフラグ (0推奨).
		nullptr,			// スレッドポンプインターフェイスへのポインター.
		ppCompiledShader,	// コンパイルされたシェーダーのデータ (out).
		ppErrors,			// コンパイル中のエラー出力 (out).
		nullptr );			// 戻り値のポインタ.
}

//----------------------------------.
// バーテックスシェーダー作成関数.
//----------------------------------.
HRESULT CreateVertexShader(
	ID3D11Device*			pDevice11,
	ID3DBlob*				pCompiledShader,
	ID3D11VertexShader**	pVertexShader )
{
	return pDevice11->CreateVertexShader(
		pCompiledShader->GetBufferPointer(),
		pCompiledShader->GetBufferSize(),
		nullptr,
		pVertexShader );
}

//----------------------------------.
// ピクセルスシェーダー作成関数.
//----------------------------------.
HRESULT CreatePixelShader(
	ID3D11Device*		pDevice11,
	ID3DBlob*			pCompiledShader,
	ID3D11PixelShader**	pPixelShader )
{
	return pDevice11->CreatePixelShader(
		pCompiledShader->GetBufferPointer(),
		pCompiledShader->GetBufferSize(),
		nullptr,
		pPixelShader );
}

//----------------------------------.
// インプットレイアウト作成関数.
//----------------------------------.
HRESULT CreateInputLayout(
	ID3D11Device*				pDevice11,
	D3D11_INPUT_ELEMENT_DESC*	pLayout,
	UINT						numElements,
	ID3DBlob*					pCompiledShader, 
	ID3D11InputLayout**			m_pVertexLayout )
{
	return pDevice11->CreateInputLayout(
		pLayout,
		numElements,
		pCompiledShader->GetBufferPointer(),
		pCompiledShader->GetBufferSize(),
		m_pVertexLayout );
}

//----------------------------------.
// コンスタントバッファ作成関数.
//----------------------------------.
HRESULT CreateConstantBuffer( 
	ID3D11Device*		pDevice11,
	ID3D11Buffer**		pConstantBuffer,
	UINT				size )
{
	D3D11_BUFFER_DESC cb;

	cb.BindFlags			= D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth			= size;
	cb.CPUAccessFlags		= D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags			= 0;
	cb.StructureByteStride	= 0;
	cb.Usage				= D3D11_USAGE_DYNAMIC;

	return pDevice11->CreateBuffer( &cb, nullptr, pConstantBuffer );
}

//----------------------------------.
// POSITIONの取得.
//----------------------------------.
D3D11_INPUT_ELEMENT_DESC GetPositionInputElement()
{
	return
	{
		"POSITION",						// 位置.
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,	// DXGIのフォーマット(32bit float型*3).
		0,
		D3D11_APPEND_ALIGNED_ELEMENT,	// データの開始位置.
		D3D11_INPUT_PER_VERTEX_DATA,
		0
	};
}

//----------------------------------.
// TEXCOORDの取得.
//----------------------------------.
D3D11_INPUT_ELEMENT_DESC GetTexcoordInputElement()
{
	return
	{
		"TEXCOORD",						// テクスチャ位置.
		0,
		DXGI_FORMAT_R32G32_FLOAT,		// DXGIのフォーマット(32bit float型*3).
		0,
		D3D11_APPEND_ALIGNED_ELEMENT,	// データの開始位置.
		D3D11_INPUT_PER_VERTEX_DATA,
		0
	};
}

};