#version 330

layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aTexCoord;

out vec2 vTexCoord;

uniform mat4 uViewProjectionModel;

void main()
{
	vTexCoord = aTexCoord;
	gl_Position = aPosition;
}