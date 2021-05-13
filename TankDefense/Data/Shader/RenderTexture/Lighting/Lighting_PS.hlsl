#include "..\RenderTexture.hlsli"

#define GBUFFER_BACK		(0)	// 色テクスチャ.
#define GBUFFER_COLOR		(1)	// 色テクスチャ.
#define GBUFFER_NORMAL		(2)	// 法線テクスチャ
#define GBUFFER_WORLD_POS	(3)	// ワールド座標テクスチャ.
#define GBUFFER_Z_DEPTH		(4)	// 深度値テクスチャ.
#define GBUFFER_DIFFUSE		(5)	// ディフーズテクスチャ.
#define GBUFFER_MAX			(6)	// テクスチャ配列の最大値.

#define LIGHT_INDEX_MAX		(2)	// ライトの最大値.

#define LIGHT_TYPE_DIRECTIONAL	(1)	// ディレクショナルライト.
#define LIGHT_TYPE_POINT		(2)	// ポイントライト.
#define LIGHT_TYPE_SPOT			(3)	// スポットライト.

//-----------------------------------------------.
// テクスチャ.
//-----------------------------------------------.
Texture2D g_GBufferColor[GBUFFER_MAX]	: register(t0);	// 色情報.

//-----------------------------------------------.
// サンプラ.
//-----------------------------------------------.
SamplerState g_SamLinear : register(s0);

//-----------------------------------------------.
// コンスタントバッファ.
//-----------------------------------------------.
cbuffer per_frame		: register( b1 )
{
	float4	g_CameraPos;						// カメラ座標.
	float4	g_LightPos[LIGHT_INDEX_MAX];		// ライト座標.
	float4	g_LightVec[LIGHT_INDEX_MAX];		// ライト方向.
	float4	g_LightColor[LIGHT_INDEX_MAX];		// ライト色(w.aをライトの強さとして使う).
	float4	g_LightConeAngle[LIGHT_INDEX_MAX];	// スポットライトのコーン角度.
	float4	g_LightType[LIGHT_INDEX_MAX];		// ライトのタイプ.
};

struct LIGHT_IN
{
	float4 LightColor;		// ライト色.
	float3 Modelcolor;		// モデル色.
	float3 NormColor;		// 法線色.
	float3 WorldPosColor;	// world座標色.
	float3 DepthColor;		// 深度色.
	float3 DiffuseColor;		// 深度色.
};

// エディレクショナルライト.
float3 DirectionalLight( float3 lightDir, float3 eyeVec, LIGHT_IN lightIn )
{
	float3 finalColor = float3( 0.0f, 0.0f, 0.0f );
	
	float NL = saturate(dot(lightIn.NormColor, lightDir));
	float3 diffuse = lightIn.Modelcolor/2.0f * NL;
	float3 reflect = normalize( 2 * NL * lightIn.NormColor - lightDir );
	float3 specular = pow(saturate(dot( reflect, eyeVec )), 4.0f);
	
	finalColor.xyz = diffuse + specular;
	finalColor.xyz *= lightIn.LightColor.rgb * lightIn.LightColor.a;
	
	return finalColor;
};

// ポイントライト.
float3 PointLight( float3 lightDir, LIGHT_IN lightIn )
{
	float3 finalColor = float3( 0.0f, 0.0f, 0.0f );
	
	float attenution = 1.0f / length(lightDir);
	if( attenution <= 0.1f ) attenution = 0.0f;
	finalColor.rgb = attenution * lightIn.LightColor.rgb * lightIn.DiffuseColor.rgb * max(dot(normalize(lightDir), lightIn.NormColor.rgb), 0) * lightIn.LightColor.a;
	
	return finalColor;
};

// スポットライト.
float3 SpotLight( float3 lightDir, float3 lightVec, float coneAngle, float3 eyeVec, LIGHT_IN lightIn )
{
	// 以下のサイトを参考.
	// https://docs.microsoft.com/ja-jp/windows/uwp/graphics-concepts/light-types.
	
	float3 finalColor = float3( 0.0f, 0.0f, 0.0f );
	
	float attenution = 1.0f /length(lightDir);
	
	float angle	= dot( normalize(lightDir), normalize(lightVec));
	float innerCone	= cos(coneAngle / 2.0);
	float outerCone	= cos(coneAngle*1.2f / 2.0);
	
	if( angle > outerCone ){
		if( angle < innerCone ){
			// innerCorn.
			attenution *= pow((angle - outerCone)/(innerCone - outerCone), 2.0f);
		} else {
			// outerCorn.
		}
		finalColor.rgb = attenution * lightIn.LightColor.rgb * lightIn.DiffuseColor.rgb * max(dot(normalize(lightDir), lightIn.NormColor.rgb), 0) * lightIn.LightColor.a;
	}
	
	return finalColor;
};


float4 PS_Main( VS_OUTPUT input ) : SV_Target
{
	float4 modelColor		= g_GBufferColor[GBUFFER_COLOR].Sample(g_SamLinear, input.Tex);
	float4 normColor		= g_GBufferColor[GBUFFER_NORMAL].Sample(g_SamLinear, input.Tex);
	float4 worldPosColor	= g_GBufferColor[GBUFFER_WORLD_POS].Sample(g_SamLinear, input.Tex);
	float4 depthColor		= g_GBufferColor[GBUFFER_Z_DEPTH].Sample(g_SamLinear, input.Tex);
	float4 diffuseColor		= g_GBufferColor[GBUFFER_DIFFUSE].Sample(g_SamLinear, input.Tex);
	
	// 0 ~ 1 を -1 ~ 1 に変換.
	normColor = normColor * 2.0f - 1.0f;
	
	float4 finalColor = modelColor;
	
	float3 eyeDir = normalize( g_CameraPos.xyz - worldPosColor.xyz );
	
	LIGHT_IN lightIn = (LIGHT_IN)0;
		
	lightIn.Modelcolor		= modelColor.rgb;
	lightIn.NormColor		= normColor.rgb;
	lightIn.WorldPosColor	= worldPosColor.rgb;
	lightIn.DepthColor		= depthColor.rgb;
	lightIn.DiffuseColor	= diffuseColor.rgb;
	for( int i = 0; i < LIGHT_INDEX_MAX; i++ ){
		float3 lightDir		= g_LightPos[i].xyz - worldPosColor.xyz;
		lightIn.LightColor	= g_LightColor[i];
		
		const int type = (int)g_LightType[i].x;
		switch( type ) {
		case LIGHT_TYPE_DIRECTIONAL:
			// ディレクショナルライト.
			finalColor.rgb += DirectionalLight( g_LightVec[i].xyz, eyeDir, lightIn );
			break;
		case LIGHT_TYPE_POINT:
			// ポイントライト.
			finalColor.rgb += PointLight( lightDir, lightIn );
			break;
		case LIGHT_TYPE_SPOT:
			// スポットライト.
			finalColor.rgb += SpotLight( lightDir, g_LightVec[i].xyz, g_LightConeAngle[i].x, eyeDir, lightIn );
			break;
		}
	}
	
	finalColor.a = 1.0f;
	
	return finalColor;
}