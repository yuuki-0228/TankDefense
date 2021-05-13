#include "..\RenderTexture.hlsli"
/*
*	↓参考サイト.
*	https://qiita.com/kanurapoison/items/1b36fdaa0a33f0d278d5
*/

#define GBUFFER_BACK		(0)	// 色テクスチャ.
#define GBUFFER_COLOR		(1)	// 色テクスチャ.
#define GBUFFER_NORMAL		(2)	// 法線テクスチャ
#define GBUFFER_WORLD_POS	(3)	// ワールド座標テクスチャ.
#define GBUFFER_Z_DEPTH		(4)	// 深度値テクスチャ.
#define GBUFFER_DIFFUSE		(5)	// ディフーズテクスチャ.
#define GBUFFER_MAX			(6)	// テクスチャ配列の最大値.

//-----------------------------------------------.
// テクスチャ.
//-----------------------------------------------.
Texture2D g_GBufferColor[GBUFFER_MAX] : register(t0); // 色情報.

//-----------------------------------------------.
// サンプラ.
//-----------------------------------------------.
SamplerState g_SamLinear : register(s0);

//-----------------------------------------------.
// コンスタントバッファ.
//-----------------------------------------------.
// ピクセルシェーダー出力.
struct PS_OUTPUT
{
	float4 Default	: SV_Target0;
	float4 LineOnly	: SV_Target1;
};

// サンプル.
struct OUTLINE_SAMPLE
{
	float NormalPower;		// 法線のサンプルの強さ.
	float NormalCompare;	// 法線の比較値.
	float DepthPower;		// 深度値のサンプルの強さ.
	float DepthCompare;		// 深度値の比較値.
};

//-----------------------------------------------.
// コンスタントバッファ.
//-----------------------------------------------.
cbuffer per_frame : register(b1)
{
	float4			Color;	// アウトラインの色.
	OUTLINE_SAMPLE	Sample;	// サンプル.
};


PS_OUTPUT PS_Main(VS_OUTPUT input)
{
	float4 modelColor		= g_GBufferColor[GBUFFER_COLOR].Sample(g_SamLinear, input.Tex);
	float4 worldPosColor	= g_GBufferColor[GBUFFER_WORLD_POS].Sample(g_SamLinear, input.Tex);
	float4 diffuseColor		= g_GBufferColor[GBUFFER_DIFFUSE].Sample(g_SamLinear, input.Tex);
	
	//----------------------------------------------------------------.
	// 輪郭線.
	//----------------------------------------------------------------.
	float s = Sample.NormalPower; // サンプリングする強さ.
	float px = s * g_vPixelSize.x, py = s * g_vPixelSize.y;
	
	
	float3 normColor = float3( 0.0f, 0.0f, 0.0f );
	normColor += g_GBufferColor[GBUFFER_NORMAL].Sample(g_SamLinear, input.Tex + clamp(float2(	 -px,	 -py), 0.0f, 1.0 )).xyz; // 左上.
	normColor += g_GBufferColor[GBUFFER_NORMAL].Sample(g_SamLinear, input.Tex + clamp(float2( 0 * px,	 -py), 0.0f, 1.0 )).xyz; // 上.
	normColor += g_GBufferColor[GBUFFER_NORMAL].Sample(g_SamLinear, input.Tex + clamp(float2(	  px,	 -py), 0.0f, 1.0 )).xyz; // 右上.
	normColor += g_GBufferColor[GBUFFER_NORMAL].Sample(g_SamLinear, input.Tex + clamp(float2(	 -px, 0 * py), 0.0f, 1.0 )).xyz; // 左.
	normColor += g_GBufferColor[GBUFFER_NORMAL].Sample(g_SamLinear, input.Tex + clamp(float2( 0 * px, 0 * py), 0.0f, 1.0 )).xyz * -8.0; // 自分.
	normColor += g_GBufferColor[GBUFFER_NORMAL].Sample(g_SamLinear, input.Tex + clamp(float2(	  px, 0 * py), 0.0f, 1.0 )).xyz; // 右.
	normColor += g_GBufferColor[GBUFFER_NORMAL].Sample(g_SamLinear, input.Tex + clamp(float2(	 -px,	  py), 0.0f, 1.0 )).xyz; // 左下.
	normColor += g_GBufferColor[GBUFFER_NORMAL].Sample(g_SamLinear, input.Tex + clamp(float2( 0 * px,	  py), 0.0f, 1.0 )).xyz; // 下.
	normColor += g_GBufferColor[GBUFFER_NORMAL].Sample(g_SamLinear, input.Tex + clamp(float2(     px,	  py), 0.0f, 1.0 )).xyz; // 右下.
	
	s = Sample.DepthPower; // サンプリングする強さ.
	px = s * g_vPixelSize.x, py = s * g_vPixelSize.y;
	float zDepth			= g_GBufferColor[GBUFFER_Z_DEPTH].Sample(g_SamLinear, input.Tex).x;
	float depthColor11		= g_GBufferColor[GBUFFER_Z_DEPTH].Sample(g_SamLinear, input.Tex + clamp(float2(		 px,	  py), 0.0f, 1.0f)).x; // 左上.
	float depthColor10		= g_GBufferColor[GBUFFER_Z_DEPTH].Sample(g_SamLinear, input.Tex + clamp(float2(		 px, 0 *  py), 0.0f, 1.0f)).x; // 上.
	float depthColor1_1		= g_GBufferColor[GBUFFER_Z_DEPTH].Sample(g_SamLinear, input.Tex + clamp(float2(		 px,	 -py), 0.0f, 1.0f)).x; // 右上.
	float depthColor01		= g_GBufferColor[GBUFFER_Z_DEPTH].Sample(g_SamLinear, input.Tex + clamp(float2( 0 *  px,	  py), 0.0f, 1.0f)).x; // 左.
	float depthColor0_1		= g_GBufferColor[GBUFFER_Z_DEPTH].Sample(g_SamLinear, input.Tex + clamp(float2( 0 *  px,	 -py), 0.0f, 1.0f)).x; // 右.
	float depthColor_11		= g_GBufferColor[GBUFFER_Z_DEPTH].Sample(g_SamLinear, input.Tex + clamp(float2(		-px,	  py), 0.0f, 1.0f)).x; // 左下.
	float depthColor_10		= g_GBufferColor[GBUFFER_Z_DEPTH].Sample(g_SamLinear, input.Tex + clamp(float2(		-px, 0 *  py), 0.0f, 1.0f)).x; // 下.
	float depthColor_1_1	= g_GBufferColor[GBUFFER_Z_DEPTH].Sample(g_SamLinear, input.Tex + clamp(float2(		-px,	 -py), 0.0f, 1.0f)).x; // 右下.
	
	// Horizontal.
	float h = (depthColor11	 *  1.0f + depthColor10	  *  2.0f)
			+ (depthColor1_1 *  1.0f + depthColor_11  * -1.0f)
			+ (depthColor_10 * -2.0f + depthColor_1_1 * -1.0f);
	// vertical.
	float v = (depthColor11	 *  1.0f + depthColor01	  *  2.0f)
			+ (depthColor_11 *  1.0f + depthColor1_1  * -1.0f)
			+ (depthColor0_1 * -2.0f + depthColor_1_1 * -1.0f);
	float depth = 1.0f - clamp(abs(h + v), 0.0f, 1.0f);
	
	PS_OUTPUT output = (PS_OUTPUT)0;
	
	// 法線情報と、深度値の情報が一定以上なら輪郭線を表示.
	if( length(normColor) >= Sample.NormalCompare || abs(depth) < Sample.DepthCompare )
	{
		output.Default = float4(Color.rgb, 1.0f);
		output.LineOnly = float4( 0.0f, 0.0f, 0.0f, 1.0f );
	}
	else
	{
		output.Default	= modelColor;
		output.LineOnly = float4( 1.0f, 1.0f, 1.0f, 1.0f );;
	}
	
	return output;
}