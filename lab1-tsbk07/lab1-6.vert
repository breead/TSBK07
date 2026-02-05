#version 150
// pipeline: program --> vertex shader --> fragment shader
in  vec3 in_Position;
uniform mat4 rotationMatrix;
uniform mat4 rotationMatrix2;
in vec3 in_Normal;
out vec3 asdf_Normal;

void main(void)
{
	gl_Position = rotationMatrix * rotationMatrix2 * vec4(in_Position, 1.0);
	asdf_Normal = in_Normal;
}
