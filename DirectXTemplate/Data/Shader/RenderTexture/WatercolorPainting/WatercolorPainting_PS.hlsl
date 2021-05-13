#include "..\RenderTexture.hlsli"

#define OUTLINE_COLOR_COLOR			(0)	// 色テクスチャ.
#define OUTLINE_COLOR_LINEONLY		(1)	// アウトラインテクスチャ
#define OUTLINE_COLOR_MAX			(2)	// テクスチャ配列の最大値.

//-----------------------------------------------.
// テクスチャ.
//-----------------------------------------------.
Texture2D g_NormTexture : register(t0);
Texture2D g_NiseTexture	: register(t1);
Texture2D g_OutLineColor[OUTLINE_COLOR_MAX] : register(t2); // 色情報.

//-----------------------------------------------.
// サンプラ.
//-----------------------------------------------.
SamplerState g_SamLinear : register(s0);

//-----------------------------------------------.
// コンスタントバッファ.
//-----------------------------------------------.
cbuffer per_frame : register(b1)
{
	float4 Color; // アウトラインの色.
};

float4 PS_Main(VS_OUTPUT input) : SV_Target
{
	float4 outLineColor = g_OutLineColor[OUTLINE_COLOR_LINEONLY].Sample(g_SamLinear, input.Tex);
	float4 noiseColor = g_NiseTexture.Sample(g_SamLinear, input.Tex);
	float4 color = g_OutLineColor[OUTLINE_COLOR_COLOR].Sample(g_SamLinear, input.Tex);
	
	float4 maskAlpha = clamp(outLineColor - (-1 + noiseColor), 0.0f, 1.0f);
	
	color = lerp(0.0f, color, maskAlpha);
	
	return float4(color.rgb, 1.0f);
}