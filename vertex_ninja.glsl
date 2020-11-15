#version 330 core 

in vec3 in_Position;
in vec3 in_Color;
out vec3 ex_Color;

uniform mat4 Transform;

void main()
{
gl_Position = Transform * vec4(in_Position, 1.0);
ex_Color = in_Color;
};
