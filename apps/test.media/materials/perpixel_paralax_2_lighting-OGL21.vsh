#version 120

#define L_DIFFUSE(lig)  vec4(lig[0].rgb,1.0)
#define L_AMBIENT(lig)  vec4(lig[1].rgb,1.0)
#define L_SPECULAR(lig) vec4(lig[2].rgb,1.0)
#define L_POS_WORLD(lig) vec3(lig[0].w,lig[1].w,lig[2].w)
#define L_POSITION(lig) lig[3].xyz
#define L_RADIUS(lig)   lig[3].w

uniform mat4 ModelViewProjMatrix;
uniform mat4 ModelViewMatrix;

varying vec3 Position;
varying vec3 Tangent;
varying vec3 Binormal;
varying vec3 Normal;
varying vec3 EyeVec;

void main(void)
{
    vec4 position = ModelViewMatrix * gl_Vertex;
    vec3 eyeVec = -position.xyz;

    Tangent  = gl_NormalMatrix * gl_MultiTexCoord2.xyz;
	Binormal = gl_NormalMatrix * gl_MultiTexCoord3.xyz;
	Normal   = gl_NormalMatrix * gl_Normal;
	EyeVec = vec3(
		dot(eyeVec, Tangent),
		dot(eyeVec, Binormal),
		dot(eyeVec, Normal));
    Position = position.xyz;
    gl_Position = ModelViewProjMatrix * gl_Vertex;
    gl_TexCoord[0] = gl_MultiTexCoord0;
}
