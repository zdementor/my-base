
#define M_DIFFUSE(mat)  mat[0].rgba
#define M_AMBIENT(mat)  mat[1].rgba
#define M_SPECULAR(mat) mat[2].rgba
#define M_EMISSIVE(mat) mat[3].rgba

#define L_DIFFUSE(lig)  lig[0].rgba
#define L_AMBIENT(lig)  lig[1].rgba
#define L_SPECULAR(lig) lig[2].rgba
#define L_POSITION(lig) lig[3].xyz
#define L_RADIUS(lig)   lig[3].w

#define LERP(v1,v2,k) (v1+(v2-(v1))*(k))

uniform float4x4 ModelViewProjMatrix;
uniform float4x4 ModelViewMatrix;
uniform float3x3 NormalMatrix;
uniform float4 GlobalAmbientColor;
uniform float4 MaterialColors[4];
uniform float MaterialShininess;
uniform float GradientLight[4];
uniform float4x4 Lighting[4];

struct VS_INPUT
{
    float4 Position : POSITION;
    float2 TexCoord0 : TEXCOORD0;
    float3 Normal : NORMAL;
};

struct VS_OUTPUT
{
    float4 PositionMVP : POSITION;
    float2 TexCoord0 : TEXCOORD0;
	float4 Color : COLOR0;
	float3 SeparateSpecular : COLOR1;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT Output;
    Output.PositionMVP  = mul(input.Position, ModelViewProjMatrix);

    float3 position = (mul(input.Position, ModelViewMatrix)).xyz;
    float3 eyeVec = -normalize(position);
    float3 normal = normalize(mul(input.Normal, NormalMatrix));

    float4 mDiffuse = M_DIFFUSE(MaterialColors);
    float4 mAmbient = M_AMBIENT(MaterialColors);
    float4 mSpecular = M_SPECULAR(MaterialColors);
    float4 mEmissive = M_EMISSIVE(MaterialColors);

    float3 specular = float3(0.0,0.0,0.0);
    float4 color = float4(0.0, 0.0, 0.0, mDiffuse.a);
	color.rgb = GlobalAmbientColor.rgb * mAmbient.rgb + mEmissive.rgb;

    // calculating lighting
    {
		int i = 0;
        float4 lDiffuse  = L_DIFFUSE(Lighting[i]);
        float4 lAmbient  = L_AMBIENT(Lighting[i]);
        float4 lSpecular = L_SPECULAR(Lighting[i]);
        float3 lPosition = L_POSITION(Lighting[i]);
        float lRadius  = L_RADIUS(Lighting[i]);
        float3 lightVec = lPosition - position;
        float lDist = length(lightVec);
        lightVec = normalize(lightVec);
        float NdotL = max(dot(normal, lightVec), 0.0);
		float idx = min(lDist / lRadius, 1.0);
		float attenuation = -idx * idx + 1.0;
        color.rgb += attenuation * (
            NdotL * lDiffuse.rgb * mDiffuse.rgb + // diffuse component
            lAmbient.rgb * mAmbient.rgb // ambient component
            );
        float3 reflVec = reflect(-lightVec, normal);
        float specPow = pow(max(dot(eyeVec, reflVec),0.0),  MaterialShininess);
        // separate specular component
        specular +=
            attenuation * specPow * mSpecular.rgb * lSpecular.rgb;
    }
    {
		int i = 1;
        float4 lDiffuse  = L_DIFFUSE(Lighting[i]);
        float4 lAmbient  = L_AMBIENT(Lighting[i]);
        float4 lSpecular = L_SPECULAR(Lighting[i]);
        float3 lPosition = L_POSITION(Lighting[i]);
        float lRadius  = L_RADIUS(Lighting[i]);
        float3 lightVec = lPosition - position;
        float lDist = length(lightVec);
        lightVec = normalize(lightVec);
        float NdotL = max(dot(normal, lightVec), 0.0);
		float idx = min(lDist / lRadius, 1.0);
		float attenuation = -idx * idx + 1.0;
        color.rgb += attenuation * (
            NdotL * lDiffuse.rgb * mDiffuse.rgb + // diffuse component
            lAmbient.rgb * mAmbient.rgb // ambient component
            );
        float3 reflVec = reflect(-lightVec, normal);
        float specPow = pow(max(dot(eyeVec, reflVec),0.0),  MaterialShininess);
        // separate specular component
        specular +=
            attenuation * specPow * mSpecular.rgb * lSpecular.rgb;
    }
    {
		int i = 2;
        float4 lDiffuse  = L_DIFFUSE(Lighting[i]);
        float4 lAmbient  = L_AMBIENT(Lighting[i]);
        float4 lSpecular = L_SPECULAR(Lighting[i]);
        float3 lPosition = L_POSITION(Lighting[i]);
        float lRadius  = L_RADIUS(Lighting[i]);
        float3 lightVec = lPosition - position;
        float lDist = length(lightVec);
        lightVec = normalize(lightVec);
        float NdotL = max(dot(normal, lightVec), 0.0);
		float idx = min(lDist / lRadius, 1.0);
		float attenuation = -idx * idx + 1.0;
        color.rgb += attenuation * (
            NdotL * lDiffuse.rgb * mDiffuse.rgb + // diffuse component
            lAmbient.rgb * mAmbient.rgb // ambient component
            );
        float3 reflVec = reflect(-lightVec, normal);
        float specPow = pow(max(dot(eyeVec, reflVec),0.0),  MaterialShininess);
        // separate specular component
        specular +=
            attenuation * specPow * mSpecular.rgb * lSpecular.rgb;
    }
    {
		int i = 3;
        float4 lDiffuse  = L_DIFFUSE(Lighting[i]);
        float4 lAmbient  = L_AMBIENT(Lighting[i]);
        float4 lSpecular = L_SPECULAR(Lighting[i]);
        float3 lPosition = L_POSITION(Lighting[i]);
        float lRadius  = L_RADIUS(Lighting[i]);
        float3 lightVec = lPosition - position;
        float lDist = length(lightVec);
        lightVec = normalize(lightVec);
        float NdotL = max(dot(normal, lightVec), 0.0);
		float idx = min(lDist / lRadius, 1.0);
		float attenuation = -idx * idx + 1.0;
        color.rgb += attenuation * (
            NdotL * lDiffuse.rgb * mDiffuse.rgb + // diffuse component
            lAmbient.rgb * mAmbient.rgb // ambient component
            );
        float3 reflVec = reflect(-lightVec, normal);
        float specPow = pow(max(dot(eyeVec, reflVec),0.0),  MaterialShininess);
        // separate specular component
        specular +=
            attenuation * specPow * mSpecular.rgb * lSpecular.rgb;
    }
	Output.SeparateSpecular = specular;
	Output.Color = color;

    Output.TexCoord0.xy = input.TexCoord0.xy;
    return Output;
}


 