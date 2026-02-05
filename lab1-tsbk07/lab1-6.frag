#version 150

in vec3 asdf_Normal;
out vec4 out_Color;

void main(void)
{
	out_Color = vec4(asdf_Normal, 1.0);
}
