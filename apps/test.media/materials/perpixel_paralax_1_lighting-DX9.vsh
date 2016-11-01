
uniform float4x4 uModelViewProjMatrix;
uniform float4x4 uModelViewMatrix;
uniform float3x3 uNormalMatrix;

struct VS_INPUT
{
    float4 Position : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord0 : TEXCOORD0;
	float3 Tangent: TANGENT;
	float3 Binormal: BINORMAL;
};

struct VS_OUTPUT
{
    float4 PositionMVP : POSITION;
    float2 TexCoord0 : TEXCOORD0;
	float2 TexCoord3 : TEXCOORD1;
	float3 Position : TEXCOORD2;
	float3 Tangent : TEXCOORD3;
	float3 Binormal : TEXCOORD4;
	float3 Normal : TEXCOORD5;
	float3 EyeVec : TEXCOORD6;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT Output;

    float3 position = (mul(input.Position, uModelViewMatrix)).xyz;
    float3 eyeVec = -position;

    Output.Tangent  = mul(input.Tangent.xyz, uNormalMatrix);
	Output.Binormal = mul(input.Binormal.xyz, uNormalMatrix);
	Output.Normal   = mul(input.Normal, uNormalMatrix);

	Output.EyeVec = (float3(
		dot(eyeVec, Output.Tangent),
		dot(eyeVec, Output.Binormal),
		dot(eyeVec, Output.Normal)));
    Output.Position = position.xyz;

    Output.TexCoord0.xy = input.TexCoord0.xy;
	Output.TexCoord3.xy = input.TexCoord0.xy;

    Output.PositionMVP  = mul(input.Position, uModelViewProjMatrix);

    return Output;
}


 