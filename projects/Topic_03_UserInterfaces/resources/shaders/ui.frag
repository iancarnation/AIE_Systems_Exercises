#version 330

in vec2 vTexCoord;

layout(location = 0) out vec4 fColor;

uniform vec4 uColor;
uniform sampler2D uDiffuseMap;

void main()
{
	fColor = texture(uDiffuseMap, vTexCoord) * uColor;
}