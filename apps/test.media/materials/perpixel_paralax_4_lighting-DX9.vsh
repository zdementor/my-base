
#define L_DIFFUSE(lig)  lig[0].rgba
#define L_AMBIENT(lig)  lig[1].rgba
#define L_SPECULAR(lig) lig[2].rgba
#define L_POSITION(lig) lig[3].xyz
#define L_RADIUS(lig)   lig[3].w

uniform float4x4 ModelViewProjMatrix;
uniform float4x4 ModelViewMatrix;
uniform float3x3 NormalMatrix;
uniform float4x4 Lighting[4];

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
	float3 Position : TEXCOORD3;
	float3 EyeVec : TEXCOORD4;
	float4 LightVec0 : TEXCOORD5;
	float4 LightVec1 : TEXCOORD6;
	float4 LightVec2 : TEXCOORD7;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT Output;

    float3 position = (mul(input.Position, ModelViewMatrix)).xyz;
    float3 eyeVec = (-position);

    float3 tangent  = (mul(input.TexCoord2.xyz, NormalMatrix));
	float3 binormal = (mul(input.TexCoord3.xyz, NormalMatrix));
	float3 normal   = (mul(input.Normal, NormalMatrix));
	Output.Position = position.xyz;
	Output.EyeVec = float3(
		dot(eyeVec, tangent),
		dot(eyeVec, binormal),
		dot(eyeVec, normal));
	{
		int i = 0;
		float3 lPosition = L_POSITION(Lighting[i]);
    	float lRadius  = L_RADIUS(Lighting[i]);
		float3 lightVec  = lPosition - position.xyz;
		lightVec = float3(
			dot(lightVec, tangent),
			dot(lightVec, binormal),
			dot(lightVec, normal));
		Output.LightVec0.x = lightVec.x;
		Output.LightVec1.x = lightVec.y; 
		Output.LightVec2.x = lightVec.z;
	}
	{
		int i = 1;
		float3 lPosition = L_POSITION(Lighting[i]);
    	float lRadius  = L_RADIUS(Lighting[i]);
		float3 lightVec  = lPosition - position.xyz;
		lightVec = float3(
			dot(lightVec, tangent),
			dot(lightVec, binormal),
			dot(lightVec, normal));
		Output.LightVec0.y = lightVec.x;
		Output.LightVec1.y = lightVec.y; 
		Output.LightVec2.y = lightVec.z;
	}
	{
		int i = 2;
		float3 lPosition = L_POSITION(Lighting[i]);
    	float lRadius  = L_RADIUS(Lighting[i]);
		float3 lightVec  = lPosition - position.xyz;
		lightVec = float3(
			dot(lightVec, tangent),
			dot(lightVec, binormal),
			dot(lightVec, normal));
		Output.LightVec0.z = lightVec.x;
		Output.LightVec1.z = lightVec.y; 
		Output.LightVec2.z = lightVec.z;
	}
	{
		int i = 3;
		float3 lPosition = L_POSITION(Lighting[i]);
    	float lRadius  = L_RADIUS(Lighting[i]);
		float3 lightVec  = lPosition - position.xyz;
		lightVec = float3(
			dot(lightVec, tangent),
			dot(lightVec, binormal),
			dot(lightVec, normal));
		Output.LightVec0.w = lightVec.x;
		Output.LightVec1.w = lightVec.y; 
		Output.LightVec2.w = lightVec.z;
	}

    Output.TexCoord0.xy = input.TexCoord0.xy;

    Output.PositionMVP  = mul(input.Position, ModelViewProjMatrix);

    return Output;
}


 