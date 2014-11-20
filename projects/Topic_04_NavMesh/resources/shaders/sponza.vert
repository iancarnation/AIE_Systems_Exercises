#version 330

layout( location = 0 ) in vec4 aPosition;
layout( location = 1 ) in vec4 aNormal;

out vec4 vWorldPosition;
out vec4 vWorldNormal;

uniform mat4 uViewProjection;
uniform mat4 uModel;
uniform mat4 uInvTransposeModel;

void main()
{
	vWorldPosition = uModel * aPosition;
	vWorldNormal = uInvTransposeModel * aNormal;
	gl_Position = uViewProjection * vWorldPosition;
}