
#define L_DIFFUSE(lig)  lig[0].rgba
#define L_AMBIENT(lig)  lig[1].rgba
#define L_SPECULAR(lig) lig[2].rgba
#define L_POSITION(lig) lig[3].xyz
#define L_RADIUS(lig)   lig[3].w

uniform float4x4 uModelViewProjMatrix;
uniform float4x4 uModelViewMatrix;
uniform float3x3 uNormalMatrix;
uniform float4x4 uLighting[4];

struct VS_INPUT
{
    float4 aPosition : POSITION;
    float2 aTCoord0 : TEXCOORD0;
	float3 aTCoord2: TEXCOORD1;
	float3 aTCoord3: TEXCOORD2;
    float3 aNormal : NORMAL;
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

    float3 position = (mul(input.aPosition, uModelViewMatrix)).xyz;
    float3 eyeVec = (-position);

    float3 tangent  = (mul(input.aTCoord2.xyz, uNormalMatrix));
	float3 binormal = (mul(input.aTCoord3.xyz, uNormalMatrix));
	float3 normal   = (mul(input.aNormal, uNormalMatrix));
	Output.Position = position.xyz;
	Output.EyeVec = float3(
		dot(eyeVec, tangent),
		dot(eyeVec, binormal),
		dot(eyeVec, normal));
	{
		int i = 0;
		float3 lPosition = L_POSITION(uLighting[i]);
    	float lRadius  = L_RADIUS(uLighting[i]);
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
		float3 lPosition = L_POSITION(uLighting[i]);
    	float lRadius  = L_RADIUS(uLighting[i]);
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
		float3 lPosition = L_POSITION(uLighting[i]);
    	float lRadius  = L_RADIUS(uLighting[i]);
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
		float3 lPosition = L_POSITION(uLighting[i]);
    	float lRadius  = L_RADIUS(uLighting[i]);
		float3 lightVec  = lPosition - position.xyz;
		lightVec = float3(
			dot(lightVec, tangent),
			dot(lightVec, binormal),
			dot(lightVec, normal));
		Output.LightVec0.w = lightVec.x;
		Output.LightVec1.w = lightVec.y; 
		Output.LightVec2.w = lightVec.z;
	}

    Output.TexCoord0.xy = input.aTCoord0.xy;

    Output.PositionMVP  = mul(input.aPosition, uModelViewProjMatrix);

    return Output;
}


 