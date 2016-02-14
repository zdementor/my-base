#version 120

#define L_POSITION(lig) lig[3].xyz
#define L_RADIUS(lig)   lig[3].w

uniform mat4 ModelViewProjMatrix;
uniform mat4 ModelViewMatrix;

uniform mat4 Lighting[4];

varying vec3 Position;
varying vec3 EyeVec;
varying vec3 LightVec[4];

void main(void)
{
    vec4 position = ModelViewMatrix * gl_Vertex;
    vec3 eyeVec = -position.xyz;

    vec3 tangent  = gl_NormalMatrix * gl_MultiTexCoord2.xyz;
	vec3 binormal = gl_NormalMatrix * gl_MultiTexCoord3.xyz;
	vec3 normal   = gl_NormalMatrix * gl_Normal;
	Position = position.xyz;
	EyeVec = vec3(
		dot(eyeVec, tangent),
		dot(eyeVec, binormal),
		dot(eyeVec, normal));
	{
		int i = 0;
		vec3 lPosition = L_POSITION(Lighting[i]);
	    float lRadius  = L_RADIUS(Lighting[i]);
		vec3 lightVec  = lPosition - position.xyz;
		LightVec[i].xyz = vec3(
			dot(lightVec, tangent),
			dot(lightVec, binormal),
			dot(lightVec, normal));
	}
	{
		int i = 1;
		vec3 lPosition = L_POSITION(Lighting[i]);
	    float lRadius  = L_RADIUS(Lighting[i]);
		vec3 lightVec  = lPosition - position.xyz;
		LightVec[i].xyz = vec3(
			dot(lightVec, tangent),
			dot(lightVec, binormal),
			dot(lightVec, normal));
	}
	{
		int i = 2;
		vec3 lPosition = L_POSITION(Lighting[i]);
	    float lRadius  = L_RADIUS(Lighting[i]);
		vec3 lightVec  = lPosition - position.xyz;
		LightVec[i].xyz = vec3(
			dot(lightVec, tangent),
			dot(lightVec, binormal),
			dot(lightVec, normal));
	}
	{
		int i = 3;
		vec3 lPosition = L_POSITION(Lighting[i]);
	    float lRadius  = L_RADIUS(Lighting[i]);
		vec3 lightVec  = lPosition - position.xyz;
		LightVec[i].xyz = vec3(
			dot(lightVec, tangent),
			dot(lightVec, binormal),
			dot(lightVec, normal));
	}

    gl_Position = ModelViewProjMatrix * gl_Vertex;
    gl_TexCoord[0] = gl_MultiTexCoord0;
}
