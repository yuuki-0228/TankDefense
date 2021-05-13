/**
* @file Common.h.
* @brief デバイス使用系の基底クラス.
* @author 福田玲也.
*/
#ifndef COMMON_H
#define COMMON_H

#include "..\Global.h"
#include "D3DX/D3DX11.h"

/***************************************
*	デバイスを使用系クラスの基底クラス.
**/
class CCommon
{
public:
	CCommon();
	virtual ~CCommon();

	// トランスフォームの設定.
	void SetTranceform( const STranceform& t ){ m_Tranceform = t; }
	// 座標の設定.
	void SetPosition( const D3DXVECTOR3& vPos ){ m_Tranceform.Position = vPos; }
	// 回転情報の設定.
	void SetRotation( const D3DXVECTOR3& vRot ){ m_Tranceform.Rotation = vRot; }
	// サイズの設定.
	void SetScale( const D3DXVECTOR3& vScale ){ m_Tranceform.Scale = vScale; }
	void SetScale( const float& fScale ){ m_Tranceform.Scale = { fScale, fScale, fScale }; }
	// 色の設定.
	void SetColor( const D3DXVECTOR4& vColor ){ m_Color = vColor; }
	// アルファ値の設定.
	void SetAlpha( const float& fAlpha ){ m_Color.w = fAlpha; }

	// ブレンドを有効:無効に設定する.
	void SetBlend( bool EnableAlpha );
	// アルファカバレージを有効:無効に設定する.
	void SetCoverage( bool EnableCoverage );
	// 深度テストを有効:無効に設定する.
	void SetDeprh( bool flag );
	// ラスタライザステート設定.
	void SetRasterizerState( const ERS_STATE& rsState );

protected:
	// デバイスの取得、各情報の初期化.
	HRESULT SetDevice( ID3D11DeviceContext* pContext11 );

protected:
	ID3D11Device*			m_pDevice11;	// デバイスオブジェクト.
	ID3D11DeviceContext*	m_pContext11;	// デバイスコンテキスト.

	STranceform m_Tranceform;	// トランスフォーム.
	D3DXVECTOR4 m_Color;		// 色.
};

#endif	// #ifndef COMMON_H.