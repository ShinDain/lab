#version 330 core 

in vec3 in_Position;
in vec3 in_Color;
out vec3 ex_Color;

uniform mat4 Transform;
uniform mat4 viewTransform;
uniform mat4 projectionTransform;

void main()
{
	gl_Position = projectionTransform * viewTransform * Transform * vec4(in_Position, 1.0f);
	ex_Color = in_Color;
}
