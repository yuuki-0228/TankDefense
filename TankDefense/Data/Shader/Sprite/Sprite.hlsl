//----------------------------.
// �e�N�X�`��.
//----------------------------.
Texture2D g_SrcTexture	: register(t0);	// �ʏ�e�N�X�`��.
Texture2D g_DestTexture	: register(t1);	// �K��摜.
Texture2D g_MaskTexture	: register(t2);	// �}�X�N�摜.
//----------------------------.
// �T���v���[.
//----------------------------.
SamplerState	g_SamLinear	: register( s0 );

//----------------------------.
// �R���X�^���g�o�b�t�@.
//----------------------------.
cbuffer global : register( b0 )
{
    matrix	g_mW		: packoffset( c0 );		// ���[���h�s��.
    matrix	g_mWVP		: packoffset( c4 );		// ���[���h�E�r���[�E�v���W�F�N�V�����s��.
    float4	g_Color		: packoffset( c8 );		// �J���[.
    float2	g_vUV		: packoffset( c9 );		// UV���W.
	float2	g_fViewPort	: packoffset( c10 );	// �r���[�|�[�g�̕��A����.
	float	g_FadeValue	: packoffset( c11 );	// �t�F�[�h�A�g�����W�V�����̒l.
};

//----------------------------.
// �\����.
//----------------------------.
struct VS_OUTPUT
{
	float4 Pos		: SV_Position;
	float4 Color	: COLOR;
	float2 Tex		: TEXCOORD;
};

VS_OUTPUT VS_Main(
	float4 Pos : POSITION,
	float2 Tex : TEXCOORD )
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Pos = mul(Pos, g_mWVP);
	output.Tex = Tex;
	// UV���W�����炷.
	output.Tex.x += g_vUV.x;
	output.Tex.y += g_vUV.y;

	return output;
}


// ���_�V�F�[�_.
VS_OUTPUT VS_MainUI(
	float4 Pos : POSITION,
	float2 Tex : TEXCOORD )
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Pos = mul(Pos, g_mW);

	// �X�N���[�����W�ɍ��킹��v�Z,
	output.Pos.x = (output.Pos.x / g_fViewPort.x) * 2.0f - 1.0f;
	output.Pos.y = 1.0f - (output.Pos.y / g_fViewPort.y) * 2.0f;

	output.Tex = Tex;

	// UV���W�����炷.
	output.Tex.x += g_vUV.x;
	output.Tex.y += g_vUV.y;

	return output;
}

// �s�N�Z���V�F�[�_.

// �ʏ�V�F�[�_�[.
float4 PS_Main(VS_OUTPUT input) : SV_Target
{
	float4 color = g_SrcTexture.Sample( g_SamLinear, input.Tex );
	color *= g_Color;
	clip(color.a);
	return color;
}

// �g�����W�V�����̃A���t�@����.
float4 PS_TAlphaOut(VS_OUTPUT input) : SV_Target
{
	float maskColor = g_MaskTexture.Sample(g_SamLinear, input.Tex).r;
	float4 texColor = g_SrcTexture.Sample(g_SamLinear, input.Tex);
	
	// �ŏI�F.
	float4 finalColor = float4(0.4f, 0.0f, 0.0f, 1.0f);
	
	// �A���t�@����.
	float maskAlpha = saturate(maskColor+(g_FadeValue*2.0f-1.0f));
	finalColor = texColor * maskAlpha + texColor * (1 - texColor.a);
	clip(finalColor.a - 0.0001f);
	
	return finalColor;
}

// �J�b�g�A�E�g.
float4 PS_TCutOut(VS_OUTPUT input) : SV_Target
{
	float maskColor = g_MaskTexture.Sample(g_SamLinear, input.Tex).r;
	float4 texColor = g_SrcTexture.Sample(g_SamLinear, input.Tex);
	
	// �ŏI�F.
	float4 finalColor = float4(0.4f, 0.0f, 0.0f, 1.0f);
	
	// �J�b�g�A�E�g.
	half maskAlpha = maskColor-(-1+g_FadeValue);
	clip(maskAlpha - 0.9999f);
	finalColor = texColor;
	
	return finalColor;
}