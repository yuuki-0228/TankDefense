//----------------------------.
// �e�N�X�`��.
//----------------------------.
Texture2D g_SrcTexture	: register( t0 );	// �ʏ�e�N�X�`��.
Texture2D g_DestTexture	: register( t1 );	// �K��摜.
Texture2D g_MaskTexture	: register( t2 );	// �}�X�N�摜.
//----------------------------.
// �T���v���[.
//----------------------------.
SamplerState	g_SamLinear	: register( s0 );

//----------------------------.
// �R���X�^���g�o�b�t�@.
//----------------------------.
cbuffer PerFrame : register( b0 )
{
	matrix	g_mW			: packoffset( c0 );		// ���[���h�s��.
	matrix	g_mWVP			: packoffset( c4 );		// ���[���h�E�r���[�E�v���W�F�N�V�����s��.
	
	float4	g_Color			: packoffset( c8 );		// �J���[.
	float2	g_vUV			: packoffset( c9 );		// UV���W.
	float2	g_fViewPort		: packoffset( c10 );	// �r���[�|�[�g�̕��A����.
	
	float2	g_CenterPos		: packoffset( c11 );	// ���S���W.
	float2	g_StartVector	: packoffset( c12 );	// �~�̊J�n�x�N�g��.
	float	g_Value			: packoffset( c13 );	// �Q�[�W���݂̒l.
	float	g_IsClockwise	: packoffset( c14 );	// ���v��肩.
};

// �~����.
#define PI	(3.14159265)

//----------------------------.
// �\����.
//----------------------------.
struct VS_OUTPUT
{
	float4 Pos		: SV_Position;
	float4 Color	: COLOR;
	float2 Tex		: TEXCOORD0;
	float2 MaskTex	: TEXCOORD1;
};

//-----------------------------.
// ���_�V�F�[�_�[.
//-----------------------------.

// 3D�`��p.
VS_OUTPUT VS_Main(
	float4 Pos : POSITION,
	float2 Tex : TEXCOORD )
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Pos = mul(Pos, g_mWVP);
	output.Tex = Tex;
	output.MaskTex = Tex;
	// UV���W�����炷.
	output.Tex.x += g_vUV.x;
	output.Tex.y += g_vUV.y;

	return output;
}

// 2D�`��p.
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
	output.MaskTex = Tex;

	// UV���W�����炷.
	output.Tex.x += g_vUV.x;
	output.Tex.y += g_vUV.y;

	return output;
}

//-----------------------------.
// �s�N�Z���V�F�[�_.
//-----------------------------.

// �T�[�N���Q�[�WPS�V�F�[�_�[.
float4 PS_CircleMain(VS_OUTPUT input) : SV_Target
{
	float4 texColor = g_SrcTexture.Sample(g_SamLinear, input.Tex);
	float maskColor = g_MaskTexture.Sample(g_SamLinear, input.MaskTex).r;
	
	// �Ώۃs�N�Z�����W�ƒ��S�̍��W�̃x�N�g�������߂�.
	float2 endVector = normalize(input.MaskTex - g_CenterPos);
	// ��̃x�N�g���̓��ς����߂�.
	// ���ς̋t�R�T�C�������߂�.
	// ��Βl�����߂�.
	float Deg = abs(acos(dot(g_StartVector, endVector)));
	
	if( g_IsClockwise >= 1.0f ){
		// 0 ~ 1 ���@���v����	>=.
		if( endVector.x >= g_StartVector.x ) Deg = PI + (PI - Deg);
	} else {
		// 0 ~ 1 ���@�����v��� <=.
		if( endVector.x <= g_StartVector.x ) Deg = PI + (PI - Deg);
	}
	
	if( Deg >= g_Value ) discard;
	
	return texColor * (1.0f-maskColor);
}