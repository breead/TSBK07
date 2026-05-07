/*
#version 150

in  vec3 inPosition;
in  vec3 inNormal;
in vec2 inTexCoord;
out vec2 texCoord;

// NY
uniform mat4 projMatrix;
uniform mat4 mdlMatrix;

void main(void)
{
	// mat3 normalMatrix1 = transpose(inverse(mat3(wrlMatrix * mdlMatrix)));
	mat3 normalMatrix1 = mat3(mdlMatrix);
	vec3 v_normal_view = normalMatrix1 * inNormal;
	texCoord = inTexCoord;
	gl_Position = projMatrix * mdlMatrix * vec4(inPosition, 1.0);
}
*/

#version 150
// pipeline: program --> vertex shader --> fragment shader
in vec3 inPosition;
uniform mat4 projMatrix;
uniform mat4 mdlMatrix; // total = worldToView * modelToWorld;
uniform mat4 wrlMatrix;

uniform vec3 p; // camera position in model coordinates

in vec3 inNormal;
out vec3 v_pos_view;
out vec3 v_normal_view;

in vec2 inTexCoord;
out vec2 texCoord;

// out vec3 exColor;

void main(void)
{
	mat3 normalMatrix = transpose(inverse(mat3(mdlMatrix)));
	v_normal_view = normalMatrix * inNormal;
	v_pos_view = vec3(mdlMatrix * vec4(inPosition, 1.0));

	vec4 camera_pos_view = mdlMatrix * vec4(p, 1.0);
	gl_Position = projMatrix * vec4(v_pos_view, 1.0);

	texCoord = inTexCoord;
	
	//exColor = 0.25*(exColor2 + exColor3 + exColor0 + exColor1); // 2 directional lights + 1 positional light
	//exColor = 0.5*(exColor2 + exColor3); // 2 directional lights

	//exColor = v_normal_view; // normal vectors in view coordinates
	//exColor = vec3(v_pos_view); // surface positions in view coordinates
}
