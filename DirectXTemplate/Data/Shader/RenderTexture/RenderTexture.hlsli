//-----------------------------------------------.
// 構造体.
//-----------------------------------------------.
// 頂点出力用.
struct VS_OUTPUT
{
	float4 Pos			: SV_Position;
	float2 Tex			: TEXCOORD0;
};

//-----------------------------------------------.
// コンスタントバッファ.
//-----------------------------------------------.
// 最初だけ渡す用.
cbuffer PerInit	: register(b0)
{
	matrix g_mW			: packoffset(c0); // ワールド行列.
	float2 g_vViewPort	: packoffset(c4); // ウィンドウサイズ.
	float2 g_vPixelSize	: packoffset(c5); // ピクセルのサイズ.
	float2 g_vWndRatio	: packoffset(c6); // ウィンドウの横幅の比率.
};