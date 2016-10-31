#version 120

#define L_DIFFUSE(lig)  vec4(lig[0].rgb,1.0)
#define L_AMBIENT(lig)  vec4(lig[1].rgb,1.0)
#define L_SPECULAR(lig) vec4(lig[2].rgb,1.0)

#define L_POSITION(lig) lig[3].xyz
#define L_RADIUS(lig)   lig[3].w

attribute vec4 aPosition;
attribute vec2 aTCoord0;
attribute vec3 aTCoord2;
attribute vec3 aTCoord3;
attribute vec3 aNormal;

uniform mat4 uModelViewProjMatrix;
uniform mat4 uModelViewMatrix;
uniform mat3 uNormalMatrix;

uniform mat4 uLighting[4];

varying vec3 Position;
varying vec3 EyeVec;
varying vec3 LightVec[4];

void main(void)
{
    vec4 position = uModelViewMatrix * aPosition;
    vec3 eyeVec = -position.xyz;

    vec3 tangent  = uNormalMatrix * aTCoord2.xyz;
	vec3 binormal = uNormalMatrix * aTCoord3.xyz;
	vec3 normal   = uNormalMatrix * aNormal;
	Position = position.xyz;
	EyeVec = vec3(
		dot(eyeVec, tangent),
		dot(eyeVec, binormal),
		dot(eyeVec, normal));
	{
		int i = 0;
		vec3 lPosition = L_POSITION(uLighting[i]);
    	float lRadius  = L_RADIUS(uLighting[i]);
		vec3 lightVec  = lPosition - position.xyz;
		LightVec[i].xyz = vec3(
			dot(lightVec, tangent),
			dot(lightVec, binormal),
			dot(lightVec, normal));
	}
	{
		int i = 1;
		vec3 lPosition = L_POSITION(uLighting[i]);
    	float lRadius  = L_RADIUS(uLighting[i]);
		vec3 lightVec  = lPosition - position.xyz;
		LightVec[i].xyz = vec3(
			dot(lightVec, tangent),
			dot(lightVec, binormal),
			dot(lightVec, normal));
	}
	{
		int i = 2;
		vec3 lPosition = L_POSITION(uLighting[i]);
    	float lRadius  = L_RADIUS(uLighting[i]);
		vec3 lightVec  = lPosition - position.xyz;
		LightVec[i].xyz = vec3(
			dot(lightVec, tangent),
			dot(lightVec, binormal),
			dot(lightVec, normal));
	}
	{
		int i = 3;
		vec3 lPosition = L_POSITION(uLighting[i]);
    	float lRadius  = L_RADIUS(uLighting[i]);
		vec3 lightVec  = lPosition - position.xyz;
		LightVec[i].xyz = vec3(
			dot(lightVec, tangent),
			dot(lightVec, binormal),
			dot(lightVec, normal));
	}
    gl_Position = uModelViewProjMatrix * aPosition;
    gl_TexCoord[0].xy = aTCoord0.xy;
}
