#version 330

in vec3 in_position;
in vec3 in_Color;
in vec2 in_uv;

out vec3 out_Color;
out vec2 TexCoord;

void main(void)
{
	gl_Position = vec4(in_position.x,in_position.y,0.9999, 1.0);
	
	out_Color = in_Color;
	
	TexCoord = in_uv;
}
