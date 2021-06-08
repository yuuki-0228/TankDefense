//----------------------------.
// テクスチャ.
//----------------------------.
Texture2D g_SrcTexture	: register( t0 );	// 通常テクスチャ.
Texture2D g_DestTexture	: register( t1 );	// 規定画像.
Texture2D g_MaskTexture	: register( t2 );	// マスク画像.
//----------------------------.
// サンプラー.
//----------------------------.
SamplerState	g_SamLinear	: register( s0 );

//----------------------------.
// コンスタントバッファ.
//----------------------------.
cbuffer PerFrame : register( b0 )
{
	matrix	g_mW			: packoffset( c0 );		// ワールド行列.
	matrix	g_mWVP			: packoffset( c4 );		// ワールド・ビュー・プロジェクション行列.
	
	float4	g_Color			: packoffset( c8 );		// カラー.
	float2	g_vUV			: packoffset( c9 );		// UV座標.
	float2	g_fViewPort		: packoffset( c10 );	// ビューポートの幅、高さ.
	
	float2	g_CenterPos		: packoffset( c11 );	// 中心座標.
	float2	g_StartVector	: packoffset( c12 );	// 円の開始ベクトル.
	float	g_Value			: packoffset( c13 );	// ゲージ現在の値.
	float	g_IsClockwise	: packoffset( c14 );	// 時計回りか.
};

// 円周率.
#define PI	(3.14159265)

//----------------------------.
// 構造体.
//----------------------------.
struct VS_OUTPUT
{
	float4 Pos		: SV_Position;
	float4 Color	: COLOR;
	float2 Tex		: TEXCOORD0;
	float2 MaskTex	: TEXCOORD1;
};

//-----------------------------.
// 頂点シェーダー.
//-----------------------------.

// 3D描画用.
VS_OUTPUT VS_Main(
	float4 Pos : POSITION,
	float2 Tex : TEXCOORD )
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Pos = mul(Pos, g_mWVP);
	output.Tex = Tex;
	output.MaskTex = Tex;
	// UV座標をずらす.
	output.Tex.x += g_vUV.x;
	output.Tex.y += g_vUV.y;

	return output;
}

// 2D描画用.
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
	output.MaskTex = Tex;

	// UV座標をずらす.
	output.Tex.x += g_vUV.x;
	output.Tex.y += g_vUV.y;

	return output;
}

//-----------------------------.
// ピクセルシェーダ.
//-----------------------------.

// サークルゲージPSシェーダー.
float4 PS_CircleMain(VS_OUTPUT input) : SV_Target
{
	float4 texColor = g_SrcTexture.Sample(g_SamLinear, input.Tex);
	float maskColor = g_MaskTexture.Sample(g_SamLinear, input.MaskTex).r;
	
	// 対象ピクセル座標と中心の座標のベクトルを求める.
	float2 endVector = normalize(input.MaskTex - g_CenterPos);
	// 二つのベクトルの内積を求める.
	// 内積の逆コサインを求める.
	// 絶対値を求める.
	float Deg = abs(acos(dot(g_StartVector, endVector)));
	
	if( g_IsClockwise >= 1.0f ){
		// 0 ~ 1 →　時計周り	>=.
		if( endVector.x >= g_StartVector.x ) Deg = PI + (PI - Deg);
	} else {
		// 0 ~ 1 →　反時計回り <=.
		if( endVector.x <= g_StartVector.x ) Deg = PI + (PI - Deg);
	}
	
	if( Deg >= g_Value ) discard;
	
	return texColor * (1.0f-maskColor);
}