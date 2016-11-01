#version 120

attribute vec4 aPosition;
attribute vec2 aTCoord0;
attribute vec3 aTangent;
attribute vec3 aBinormal;
attribute vec3 aNormal;

uniform mat4 uModelViewProjMatrix;
uniform mat4 uModelViewMatrix;
uniform mat3 uNormalMatrix;

varying vec2 TexCoord0;
varying vec2 TexCoord3;
varying vec3 Position;
varying vec3 Tangent;
varying vec3 Binormal;
varying vec3 Normal;
varying vec3 EyeVec;

void main(void)
{
    vec4 position = uModelViewMatrix * aPosition;
    vec3 eyeVec = -position.xyz;

    Tangent  = uNormalMatrix * aTangent.xyz;
	Binormal = uNormalMatrix * aBinormal.xyz;
	Normal   = uNormalMatrix * aNormal;
	EyeVec = vec3(
		dot(eyeVec, Tangent),
		dot(eyeVec, Binormal),
		dot(eyeVec, Normal));
    Position = position.xyz;

    TexCoord0 = aTCoord0.xy;
	TexCoord3 = aTCoord0.xy;

    gl_Position = uModelViewProjMatrix * aPosition;
}
