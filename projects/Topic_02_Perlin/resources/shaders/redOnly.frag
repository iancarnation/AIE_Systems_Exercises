#version 330

in vec2 teTexCoord;

layout(location = 0) out vec4 fColor;

uniform sampler2D uDiffuseMap;

void main()
{
	fColor.rgb = texture(uDiffuseMap, teTexCoord).rrr;
	fColor.a = 1;
}