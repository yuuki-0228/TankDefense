#include "..\RenderTexture.hlsli"

//-----------------------------------------------.
// テクスチャ.
//-----------------------------------------------.
Texture2D g_Texture : register(t0); // 色情報.

//-----------------------------------------------.
// サンプラ.
//-----------------------------------------------.
SamplerState g_SamLinear : register(s0);

float4 PS_Main(VS_OUTPUT input) : SV_Target
{
	float4 offset = g_vPixelSize.xyxy * float2(-1.0f, 1.0f).xxyy;
	half3 sum = 
		g_Texture.Sample( g_SamLinear, input.Tex*g_vWndRatio + offset.xy ).rgb + 
		g_Texture.Sample( g_SamLinear, input.Tex*g_vWndRatio + offset.zy ).rgb + 
		g_Texture.Sample( g_SamLinear, input.Tex*g_vWndRatio + offset.xw ).rgb + 
		g_Texture.Sample( g_SamLinear, input.Tex*g_vWndRatio + offset.zw ).rgb;
	
	return float4( sum*0.125f, 1.0f );
}