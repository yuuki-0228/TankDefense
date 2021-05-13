/***************************************************************************************************
*	SkinMeshCode Version 2.00
*	LastUpdate	: 2019/10/09
*	Auther		: an.
**/

#include "MeshPS.hlsli"

//定義.
#define MAX_BONE_MATRICES (255)

//ボーンのポーズ行列が入る.
cbuffer per_bones		: register( b3 )
{
	matrix g_mConstBoneWorld[MAX_BONE_MATRICES];
};

//スキニング後の頂点・法線が入る.
struct Skin
{
	float4 Pos;
	float4 Norm;
	float4 Tangent;
	float4 Binorm;
};
//バーテックスバッファーの入力.
struct VSSkinIn
{
	float3 Pos		: POSITION;		//位置.  
	float3 Norm		: NORMAL;		//頂点法線.
	float2 Tex		: TEXCOORD;		//テクスチャー座標.
	float3 Tangent	: TANGENT;
	float3 Binorm	: BINORMAL;
	uint4  Bones	: BONE_INDEX;	//ボーンのインデックス.
	float4 Weights	: BONE_WEIGHT;	//ボーンの重み.
};

//指定した番号のボーンのポーズ行列を返す.
//サブ関数（バーテックスシェーダーで使用）.
matrix FetchBoneMatrix( uint iBone )
{
	return g_mConstBoneWorld[iBone];
}


//頂点をスキニング（ボーンにより移動）する.
//サブ関数（バーテックスシェーダーで使用）.
Skin SkinVert( VSSkinIn Input )
{
	Skin Output = (Skin)0;

	float4 Pos		= float4(Input.Pos,1);
	float4 Norm		= float4(Input.Norm,0);
	float4 Tangent	= float4(Input.Tangent,0);
	float4 Binorm	= float4(Input.Binorm,0);
	//ボーン0.
	uint iBone		=Input.Bones.x;
	float fWeight	= Input.Weights.x;
	matrix m	=  FetchBoneMatrix( iBone );
	Output.Pos		+= fWeight * mul( Pos, m );
	Output.Norm		+= fWeight * mul( Norm, m );
	Output.Tangent	+= fWeight * mul( Tangent, m );
	Output.Binorm	+= fWeight * mul( Binorm, m );
	//ボーン1.
	iBone	= Input.Bones.y;
	fWeight	= Input.Weights.y;
	m		= FetchBoneMatrix( iBone );
	Output.Pos		+= fWeight * mul( Pos, m );
	Output.Norm		+= fWeight * mul( Norm, m );
	Output.Tangent	+= fWeight * mul( Tangent, m );
	Output.Binorm	+= fWeight * mul( Binorm, m );
	//ボーン2.
	iBone	= Input.Bones.z;
	fWeight	= Input.Weights.z;
	m		= FetchBoneMatrix( iBone );
	Output.Pos		+= fWeight * mul( Pos, m );
	Output.Norm		+= fWeight * mul( Norm, m );
	Output.Tangent	+= fWeight * mul( Tangent, m );
	Output.Binorm	+= fWeight * mul( Binorm, m );
	//ボーン3.
	iBone	= Input.Bones.w;
	fWeight	= Input.Weights.w;
	m		= FetchBoneMatrix( iBone );
	Output.Pos		+= fWeight * mul( Pos, m );
	Output.Norm		+= fWeight * mul( Norm, m );
	Output.Tangent	+= fWeight * mul( Tangent, m );
	Output.Binorm	+= fWeight * mul( Binorm, m );

	return Output;
}

// バーテックスシェーダ.
VS_OUTPUT VS_Main( VSSkinIn input )
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	Skin vSkinned = SkinVert(input);

	output.Pos		= mul(vSkinned.Pos, g_mWVP);
	output.PosW		= mul(vSkinned.Pos, g_mW).xyz;
	output.Normal	= normalize(mul(vSkinned.Norm,		g_mW)).xyz;
	output.Tangent	= normalize(mul(vSkinned.Tangent,	g_mW)).xyz;
	output.Binormal	= normalize(mul(vSkinned.Binorm,	g_mW)).xyz;
	output.LightDir	= g_vLightDir.xyz;
	output.EyeVector	= normalize( g_vCamPos.xyz - output.PosW.xyz );
	output.Tex			= input.Tex;

	return output;
}
