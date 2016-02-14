#version 120

uniform mat4 ModelViewProjMatrix;
uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;

varying vec2 TexCoord0;
varying vec2 TexCoord3;
varying vec3 Position;
varying vec3 Tangent;
varying vec3 Binormal;
varying vec3 Normal;
varying vec3 EyeVec;

void main(void)
{
    vec4 position = ModelViewMatrix * gl_Vertex;
    vec3 eyeVec = -position.xyz;

    Tangent  = NormalMatrix * gl_MultiTexCoord2.xyz;
	Binormal = NormalMatrix * gl_MultiTexCoord3.xyz;
	Normal   = NormalMatrix * gl_Normal;
	EyeVec = vec3(
		dot(eyeVec, Tangent),
		dot(eyeVec, Binormal),
		dot(eyeVec, Normal));
    Position = position.xyz;

    TexCoord0 = gl_MultiTexCoord0.xy;
	TexCoord3 = gl_MultiTexCoord0.xy;

    gl_Position = ModelViewProjMatrix * gl_Vertex;
}
