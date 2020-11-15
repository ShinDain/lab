#version 330 core

in vec3 out_Color;
in vec2 TexCoord;

out vec4 ex_color;

uniform sampler2D outTexture;

void main(void)
{	
	ex_color = texture(outTexture, TexCoord) * vec4(out_Color,1.0);
}