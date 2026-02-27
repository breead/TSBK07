#version 150
// pipeline: program --> vertex shader --> fragment shader
in  vec3 in_Position;
uniform mat4 projectionMatrix;
uniform mat4 mdlMatrix;
uniform mat4 wrlMatrix;

uniform vec3 p; // camera position in model coordinates

in vec3 in_Normal;
out vec3 ex_Normal;

in vec2 inTexCoord;
out vec2 texCoord;

out vec3 exColor;



uniform float t;

// här eller i fragment?
uniform vec3 lightSourcesDirPosArr[4];
uniform vec3 lightSourcesColorArr[4];
uniform float specularExponent;
uniform bool isDirectional[4];

void main(void)
{
	mat3 normalMatrix1 = mat3(mdlMatrix);
	gl_Position = projectionMatrix * wrlMatrix * mdlMatrix * vec4(in_Position, 1.0);
	vec4 surface_pos_view = wrlMatrix * mdlMatrix * vec4(in_Position, 1.0);
	vec4 camera_pos_view = wrlMatrix * mdlMatrix * vec4(p, 1.0);
	// nånting = wrlMatrix * lightSourcesDisPosArr[0];
	ex_Normal = normalMatrix1 * in_Normal;
	texCoord = inTexCoord;
	
	vec3 light = lightSourcesDirPosArr[2];
	vec3 color = lightSourcesColorArr[2];
	float shade;
	shade = dot(normalize(ex_Normal), light);
	exColor = vec3(shade);
	exColor.x = exColor.x * color.x;
	exColor.y = exColor.y * color.y;
	exColor.z = exColor.z * color.z;

	// exColor = ex_Normal;
	exColor = vec3(surface_pos_view);
}
