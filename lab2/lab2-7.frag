#version 150

in vec3 ex_Normal;
out vec4 out_Color;
in vec2 texCoord;
uniform sampler2D texUnit;
in vec3 exColor;

void main(void)
{
	//vec4 color;
	//color = texture(texUnit, texCoord*2);
	out_Color = vec4(exColor, 1.0);
}
