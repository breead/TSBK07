#version 150
// pipeline: program --> vertex shader --> fragment shader
in  vec3 in_Position;
uniform mat4 projectionMatrix;
uniform mat4 mdlMatrix;
uniform mat4 wrlMatrix;

in vec3 in_Normal;
out vec3 ex_Normal;

in vec2 inTexCoord;
out vec2 texCoord;

out vec3 exColor;

uniform float t;

void main(void)
{
	mat3 normalMatrix1 = mat3(mdlMatrix);
	gl_Position = projectionMatrix * wrlMatrix * mdlMatrix * vec4(in_Position, 1.0);
	ex_Normal = normalMatrix1 * in_Normal;
	texCoord = inTexCoord;
	
	const vec3 light = vec3(0.58, 0.58, 0.58);
	float shade;
	shade = dot(normalize(ex_Normal), light);
	exColor = vec3(shade);
}
