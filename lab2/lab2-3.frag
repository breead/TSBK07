#version 150

in vec3 ex_Normal;
out vec4 out_Color;
in vec2 texCoord;
uniform sampler2D texUnit;

void main(void)
{
	vec4 color;
	color = texture(texUnit, texCoord*2);
	out_Color = vec4(color);
}
