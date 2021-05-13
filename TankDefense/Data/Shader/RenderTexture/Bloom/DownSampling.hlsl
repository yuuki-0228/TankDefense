#include "..\RenderTexture.hlsli"

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
cbuffer per_frame : register(b1)
{
	float4 g_SoftKneePram;
	float4 g_Intensity;
};

float4 PS_Main(VS_OUTPUT input) : SV_Target
{
	// 描画されたモデルなどの色情報を取得.
	float4 color = float4( g_GBufferColor[GBUFFER_COLOR].Sample(g_SamLinear, input.Tex).rgb, 1.0f );
	
	// テクスチャの明度を落とす.
	// --- softknee ---.
	// https://light11.hatenadiary.com/entry/2018/02/15/011155.
	// https://light11.hatenadiary.com/entry/2018/03/15/000022.
	
	// 明度を落とした色.
	half source = max(color.r, max(color.g, color.b));
	
	half soft = clamp(source - g_SoftKneePram.y, 0, g_SoftKneePram.z);
	soft = soft * soft * g_SoftKneePram.w;
	// 計算式.
	//		   (min(knee*2[param.z], max(0, source-threshold+knee[param.y])))2.
	//	soft = ---------------------------------------------------------------.
	//							4 * knee + 0.00001[param.w].
	
	half contribution = max(soft, source-g_SoftKneePram.x);
	contribution /= max(source, 0.0001f);
	// 計算式.
	//				 (max(source - threshold[param.x], soft)).
	// contribution = ---------------------------------------.
	//							max(source, 0.0001f).
	return float4(color.rgb*contribution, 1.0f)*g_Intensity.x;
}