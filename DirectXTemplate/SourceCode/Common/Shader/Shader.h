/**
* @file Shader.h.
* @brief シェーダー作成用関数をまとめたヘッダー.
* @author 福田玲也.
*/
#ifndef SHADER_H
#define SHADER_H

#include "..\Common.h"

namespace shader
{
// シェーダー読込関数.
HRESULT InitShader(
	const char* filePath,
	const char* entryPoint,
	const char* shaderModel,
	UINT compileFlag,
	ID3DBlob** ppCompiledShader,
	ID3DBlob** ppErrors );

// バーテックスシェーダー作成関数.
HRESULT CreateVertexShader(
	ID3D11Device*			pDevice11,
	ID3DBlob*				pCompiledShader,
	ID3D11VertexShader**	pVertexShader );

// ピクセルスシェーダー作成関数.
HRESULT CreatePixelShader(
	ID3D11Device*		pDevice11,
	ID3DBlob*			pCompiledShader,
	ID3D11PixelShader**	pPixelShader );

// インプットレイアウト作成関数.
HRESULT CreateInputLayout(
	ID3D11Device*				pDevice11,
	D3D11_INPUT_ELEMENT_DESC*	pLayout,
	UINT						numElements,
	ID3DBlob*					pCompiledShader, 
	ID3D11InputLayout**			m_pVertexLayout );

// コンスタントバッファ作成関数.
HRESULT CreateConstantBuffer( 
	ID3D11Device*	pDevice11,
	ID3D11Buffer**	pConstantBuffer,
	UINT			size );

// POSITIONの取得.
D3D11_INPUT_ELEMENT_DESC GetPositionInputElement();
// TEXCOORDの取得.
D3D11_INPUT_ELEMENT_DESC GetTexcoordInputElement();

};
#endif	// #ifndef SHADER_H.