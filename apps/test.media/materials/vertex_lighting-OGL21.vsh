// This file automaticaly generated by MyEngine Shader Generator
// Date/time: 01/16/11 14:01:54
// DON'T EDIT this file or you may lost your changes!

#version 120

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

uniform mat4 ModelViewProjMatrix;
uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform vec4 GlobalAmbientColor;
uniform vec4 MaterialColors[4];
uniform float MaterialShininess;
uniform mat4 Lighting[4];

varying vec3 SeparateSpecular;

void main(void)
{
    vec3 position = vec3(ModelViewMatrix * gl_Vertex);
    vec3 eyeVec = -normalize(position);
    vec3 normal = normalize(NormalMatrix * gl_Normal);

    vec4 mDiffuse = M_DIFFUSE(MaterialColors);
    vec4 mAmbient = M_AMBIENT(MaterialColors);
    vec4 mSpecular = M_SPECULAR(MaterialColors);
    vec4 mEmissive = M_EMISSIVE(MaterialColors);

    vec3 specular = vec3(0.0,0.0,0.0);
    vec4 color = vec4(0.0, 0.0, 0.0, mDiffuse.a);
    color.rgb = GlobalAmbientColor.rgb * mAmbient.rgb + mEmissive.rgb;
    
    // calculating lighting
    {
		int i = 0;
        vec4 lDiffuse  = L_DIFFUSE(Lighting[i]);
        vec4 lAmbient  = L_AMBIENT(Lighting[i]);
        vec4 lSpecular = L_SPECULAR(Lighting[i]);
        vec3 lPosition = L_POSITION(Lighting[i]);
        float lRadius  = L_RADIUS(Lighting[i]);
        vec3 lightVec = lPosition - position;
        float lDist = length(lightVec);
        lightVec = normalize(lightVec);
        float NdotL = max(dot(normal, lightVec), 0.0);
		float idx = min(lDist / lRadius, 1.0);
		float attenuation = -idx * idx + 1.0;
        color.rgb += attenuation * (
            NdotL * lDiffuse.rgb * mDiffuse.rgb + // diffuse component
            lAmbient.rgb * mAmbient.rgb // ambient component
            );
		vec3 reflVec = reflect(-lightVec, normal);
        float specPow = pow(max(dot(eyeVec, reflVec),0.0),  MaterialShininess);
        // separate specular component
        specular +=
            attenuation * specPow * mSpecular.rgb * lSpecular.rgb;
    }
    {
		int i = 1;
        vec4 lDiffuse  = L_DIFFUSE(Lighting[i]);
        vec4 lAmbient  = L_AMBIENT(Lighting[i]);
        vec4 lSpecular = L_SPECULAR(Lighting[i]);
        vec3 lPosition = L_POSITION(Lighting[i]);
        float lRadius  = L_RADIUS(Lighting[i]);
        vec3 lightVec = lPosition - position;
        float lDist = length(lightVec);
        lightVec = normalize(lightVec);
        float NdotL = max(dot(normal, lightVec), 0.0);
		float idx = min(lDist / lRadius, 1.0);
		float attenuation = -idx * idx + 1.0;
        color.rgb += attenuation * (
            NdotL * lDiffuse.rgb * mDiffuse.rgb + // diffuse component
            lAmbient.rgb * mAmbient.rgb // ambient component
            );
		vec3 reflVec = reflect(-lightVec, normal);
        float specPow = pow(max(dot(eyeVec, reflVec),0.0),  MaterialShininess);
        // separate specular component
        specular +=
            attenuation * specPow * mSpecular.rgb * lSpecular.rgb;
    }
    {
		int i = 2;
        vec4 lDiffuse  = L_DIFFUSE(Lighting[i]);
        vec4 lAmbient  = L_AMBIENT(Lighting[i]);
        vec4 lSpecular = L_SPECULAR(Lighting[i]);
        vec3 lPosition = L_POSITION(Lighting[i]);
        float lRadius  = L_RADIUS(Lighting[i]);
        vec3 lightVec = lPosition - position;
        float lDist = length(lightVec);
        lightVec = normalize(lightVec);
        float NdotL = max(dot(normal, lightVec), 0.0);
		float idx = min(lDist / lRadius, 1.0);
		float attenuation = -idx * idx + 1.0;
        color.rgb += attenuation * (
            NdotL * lDiffuse.rgb * mDiffuse.rgb + // diffuse component
            lAmbient.rgb * mAmbient.rgb // ambient component
            );
		vec3 reflVec = reflect(-lightVec, normal);
        float specPow = pow(max(dot(eyeVec, reflVec),0.0),  MaterialShininess);
        // separate specular component
        specular +=
            attenuation * specPow * mSpecular.rgb * lSpecular.rgb;
    }
    {
		int i = 3;
        vec4 lDiffuse  = L_DIFFUSE(Lighting[i]);
        vec4 lAmbient  = L_AMBIENT(Lighting[i]);
        vec4 lSpecular = L_SPECULAR(Lighting[i]);
        vec3 lPosition = L_POSITION(Lighting[i]);
        float lRadius  = L_RADIUS(Lighting[i]);
        vec3 lightVec = lPosition - position;
        float lDist = length(lightVec);
        lightVec = normalize(lightVec);
        float NdotL = max(dot(normal, lightVec), 0.0);
		float idx = min(lDist / lRadius, 1.0);
		float attenuation = -idx * idx + 1.0;
        color.rgb += attenuation * (
            NdotL * lDiffuse.rgb * mDiffuse.rgb + // diffuse component
            lAmbient.rgb * mAmbient.rgb // ambient component
            );
		vec3 reflVec = reflect(-lightVec, normal);
        float specPow = pow(max(dot(eyeVec, reflVec),0.0),  MaterialShininess);
        // separate specular component
        specular +=
            attenuation * specPow * mSpecular.rgb * lSpecular.rgb;
    }
	SeparateSpecular = specular;

    gl_FrontColor = color;
    gl_Position = ModelViewProjMatrix * gl_Vertex;
    gl_TexCoord[0] = gl_MultiTexCoord0;
}
