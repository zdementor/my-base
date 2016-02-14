
uniform float4x4 ModelViewProjMatrix;
uniform float4x4 ModelViewMatrix;
uniform float3x3 NormalMatrix;

struct VS_INPUT
{
    float4 Position : POSITION;
    float2 TexCoord0 : TEXCOORD0;
	float3 TexCoord2: TEXCOORD1;
	float3 TexCoord3: TEXCOORD2;
    float3 Normal : NORMAL;
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

    float3 position = (mul(input.Position, ModelViewMatrix)).xyz;
    float3 eyeVec = -position;

    Output.Tangent  = mul(input.TexCoord2.xyz, NormalMatrix);
	Output.Binormal = mul(input.TexCoord3.xyz, NormalMatrix);
	Output.Normal   = mul(input.Normal, NormalMatrix);

	Output.EyeVec = (float3(
		dot(eyeVec, Output.Tangent),
		dot(eyeVec, Output.Binormal),
		dot(eyeVec, Output.Normal)));
    Output.Position = position.xyz;

    Output.TexCoord0.xy = input.TexCoord0.xy;
	Output.TexCoord3.xy = input.TexCoord0.xy;

    Output.PositionMVP  = mul(input.Position, ModelViewProjMatrix);

    return Output;
}


 