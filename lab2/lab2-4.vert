#version 150
// pipeline: program --> vertex shader --> fragment shader
in  vec3 in_Position;
uniform mat4 rotationMatrix;
uniform mat4 rotationMatrix2;
uniform mat4 projectionMatrix;
uniform mat4 mdlMatrix;
uniform mat4 wrlMatrix;

in vec3 in_Normal;
out vec3 ex_Normal;

in vec2 inTexCoord;
out vec2 texCoord;


uniform float t;

void main(void)
{
	// gl_Position = rotationMatrix * rotationMatrix2 * vec4(in_Position, 1.0);
	gl_Position = projectionMatrix * wrlMatrix* mdlMatrix * vec4(in_Position, 1.0);
	ex_Normal = in_Normal;
	texCoord = inTexCoord;
}
