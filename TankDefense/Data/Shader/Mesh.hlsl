#include "MeshPS.hlsli"

// 頂点シェーダーの出力パラメータ.
struct VS_INPUT
{
	float3 Pos		: POSITION;
	float3 Norm		: NORMAL;
	float2 Tex		: TEXCOORD;
	float3 Tangent	: TANGENT;
	float3 Binorm	: BINORMAL;
};

//-------------------------------------------------
//	頂点シェーダー.
//-------------------------------------------------
VS_OUTPUT VS_Main( VS_INPUT input )
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	
	output.Pos		= mul(float4(input.Pos, 1.0f),	g_mWVP);
	output.PosW		= mul(float4(input.Pos, 1.0f),	g_mW).xyz;
	output.Normal	= normalize(mul(float4(input.Norm, 1.0f),	g_mW)).xyz;
	output.Tangent	= normalize(mul(float4(input.Tangent, 1.0f),g_mW)).xyz;
	output.Binormal	= normalize(mul(float4(input.Binorm, 1.0f),	g_mW)).xyz;
	output.LightDir	= g_vLightDir.xyz;
	output.EyeVector = normalize( g_vCamPos.xyz - output.PosW.xyz );
	output.Tex = input.Tex;

	return output;
}