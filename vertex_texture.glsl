#version 330

in vec3 in_position;
in vec3 in_normal;
in vec2 in_uv;

out vec3 v2f_normal;
out vec3 v2f_worldPos; 
out vec2 TexCoord;

uniform mat4 Transform;
uniform mat4 viewTransform;
uniform mat4 projectionTransform;

void main(void)
{
	vec4 worldPos = Transform * vec4(in_position, 1.0);
	gl_Position = projectionTransform * viewTransform * worldPos;

	vec4 worldNormal = Transform * vec4(in_normal, 0.0);
	v2f_normal = vec3(worldNormal.x, worldNormal.y, worldNormal.z);
	v2f_worldPos = vec3(worldPos.x, worldPos.y, worldPos.z);

	TexCoord = in_uv;
}
