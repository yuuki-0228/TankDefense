#include "RenderTexture.hlsli"

#define BLOOM_SAMPLE_NUM (5)

//-----------------------------------------------.
// テクスチャ.
//-----------------------------------------------.
Texture2D g_Texture							: register(t0);	// 色情報.
Texture2D g_Back							: register(t1);	// 色情報.
Texture2D g_TextureBloom[BLOOM_SAMPLE_NUM]	: register(t2);	// ブルーム情報.

//-----------------------------------------------.
// サンプラ.
//-----------------------------------------------.
SamplerState g_SamLinear : register(s0);


float4 PS_Main( VS_OUTPUT input ) : SV_Target
{
	float4 color = g_Texture.Sample(g_SamLinear, input.Tex);
	
	// ブルームのサンプル数文.
	for( int i = 0; i < BLOOM_SAMPLE_NUM; i++ ){
		color += g_TextureBloom[i].Sample(g_SamLinear, input.Tex);
	}
	
	// 画像や、エフェクトなどを描画.
	float4 t = g_Back.Sample(g_SamLinear, input.Tex);
	if( t.a > 0.0f ) color.rgb = t.rgb;
	
	return color;
}