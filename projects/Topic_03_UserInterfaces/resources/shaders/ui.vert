#version 330

layout(location = 0) in vec3 aPosition;

out vec2 vTexCoord;

uniform vec2 uTexCoords[4];
uniform vec2 uDimensions;
uniform mat3 uTransform;
uniform mat4 uProjection;

void main()
{
	vec3 p = aPosition;
	p.xy *= uDimensions;

	vTexCoord = uTexCoords[gl_VertexID];

	gl_Position = uProjection * vec4(uTransform * p, 1);
}