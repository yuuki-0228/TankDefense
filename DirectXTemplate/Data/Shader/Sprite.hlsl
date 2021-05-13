//----------------------------.
// テクスチャ.
//----------------------------.
Texture2D g_SrcTexture	: register(t0);	// 通常テクスチャ.
Texture2D g_DestTexture	: register(t1);	// 規定画像.
Texture2D g_MaskTexture	: register(t2);	// マスク画像.
//----------------------------.
// サンプラー.
//----------------------------.
SamplerState	g_SamLinear	: register( s0 );

//----------------------------.
// コンスタントバッファ.
//----------------------------.
cbuffer global : register( b0 )
{
    matrix	g_mW		: packoffset( c0 );		// ワールド行列.
    matrix	g_mWVP		: packoffset( c4 );		// ワールド・ビュー・プロジェクション行列.
    float4	g_Color		: packoffset( c8 );		// カラー.
    float2	g_vUV		: packoffset( c9 );		// UV座標.
	float2	g_fViewPort	: packoffset( c10 );	// ビューポートの幅、高さ.
	float	g_FadeValue	: packoffset( c11 );	// フェード、トランジションの値.
};

//----------------------------.
// 構造体.
//----------------------------.
struct VS_OUTPUT
{
	float4 Pos		: SV_Position;
	float4 Color	: COLOR;
	float2 Tex		: TEXCOORD;
};

VS_OUTPUT VS_Main(
	float4 Pos : POSITION,
	float2 Tex : TEXCOORD )
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Pos = mul(Pos, g_mWVP);
	output.Tex = Tex;
	// UV座標をずらす.
	output.Tex.x += g_vUV.x;
	output.Tex.y += g_vUV.y;

	return output;
}


// 頂点シェーダ.
VS_OUTPUT VS_MainUI(
	float4 Pos : POSITION,
	float2 Tex : TEXCOORD )
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Pos = mul(Pos, g_mW);

	// スクリーン座標に合わせる計算,
	output.Pos.x = (output.Pos.x / g_fViewPort.x) * 2.0f - 1.0f;
	output.Pos.y = 1.0f - (output.Pos.y / g_fViewPort.y) * 2.0f;

	output.Tex = Tex;

	// UV座標をずらす.
	output.Tex.x += g_vUV.x;
	output.Tex.y += g_vUV.y;

	return output;
}

// ピクセルシェーダ.

// 通常シェーダー.
float4 PS_Main(VS_OUTPUT input) : SV_Target
{
	float4 color = g_SrcTexture.Sample( g_SamLinear, input.Tex );
	color *= g_Color;
	clip(color.a);
	return color;
}

// トランジションのアルファ抜き.
float4 PS_TAlphaOut(VS_OUTPUT input) : SV_Target
{
	float maskColor = g_MaskTexture.Sample(g_SamLinear, input.Tex).r;
	float4 texColor = g_SrcTexture.Sample(g_SamLinear, input.Tex);
	
	// 最終色.
	float4 finalColor = float4(0.4f, 0.0f, 0.0f, 1.0f);
	
	// アルファ抜け.
	float maskAlpha = saturate(maskColor+(g_FadeValue*2.0f-1.0f));
	finalColor = texColor * maskAlpha + texColor * (1 - texColor.a);
	clip(finalColor.a - 0.0001f);
	
	return finalColor;
}

// カットアウト.
float4 PS_TCutOut(VS_OUTPUT input) : SV_Target
{
	float maskColor = g_MaskTexture.Sample(g_SamLinear, input.Tex).r;
	float4 texColor = g_SrcTexture.Sample(g_SamLinear, input.Tex);
	
	// 最終色.
	float4 finalColor = float4(0.4f, 0.0f, 0.0f, 1.0f);
	
	// カットアウト.
	half maskAlpha = maskColor-(-1+g_FadeValue);
	clip(maskAlpha - 0.9999f);
	finalColor = texColor;
	
	return finalColor;
}