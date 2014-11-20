#version 330

in vec4 vWorldPosition;
in vec4 vWorldNormal;

layout( location = 0 ) out vec4 fColor;

void main()
{
	vec3 color = vec3(1);
	if (mod( vWorldPosition.x, 1.0 ) < 0.05f ||
		mod( vWorldPosition.y, 1.0 ) < 0.05f ||
		mod( vWorldPosition.z, 1.0 ) < 0.05f)
		color = vec3(0);

	float d = max( 0, dot( normalize(vec3(1,1,1)), normalize(vWorldNormal.xyz) ) ) * 0.75f;

	fColor.rgb = color * 0.25f + color * d;
	fColor.a = 1;
}