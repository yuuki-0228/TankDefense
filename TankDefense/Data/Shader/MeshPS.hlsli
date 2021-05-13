// �e�N�X�`��.
Texture2D		g_Texture		: register(t0);
Texture2D		g_NormTexture	: register(t1);
// �T���v��.
SamplerState	g_SamLinear	: register(s0);

//�R���X�^���g�o�b�t�@(���b�V������).
cbuffer per_mesh		: register( b0 )
{
	matrix	g_mW;		// ���[���h�s��.
	matrix	g_mWVP;		// ���[���h����ˉe�܂ł̕ϊ��s��.
	float4	g_vColor;	// �F.
};
// �}�e���A�����ɕύX.
cbuffer per_material : register(b1)
{
	float4	g_vAmbient;		// �A���r�G���g�F(���F).
	float4	g_vDiffuse;		// �f�B�t���[�Y�F(�g�U���ːF).
	float4	g_vSpecular;	// �X�y�L�����F(���ʔ��ːF).
};
// �t���[�����ɕύX.
cbuffer per_frame : register(b2)
{
	float4	g_vCamPos;		// �J�����ʒu(���_�ʒu).
	float4	g_vLightDir;	// ���C�g�̕����x�N�g��.
	float4	g_vIntensity;	// ���C�g�̋���.
};

// ���_�V�F�[�_�[�̏o�̓p�����[�^�[.
struct VS_OUTPUT
{
	float4	Pos			: SV_Position;
	float3	Binormal	: BINORMAL0;
	float3	Tangent		: TANGENT0;
	float3	PosW		: TEXCOORD0;
	float3	LightDir	: TEXCOORD1;
	float3	Normal		: TEXCOORD2;
	float3	EyeVector	: TEXCOORD3;
	float2	Tex			: TEXCOORD4;
};