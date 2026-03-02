#version 150
// pipeline: program --> vertex shader --> fragment shader
in vec3 in_Position;
uniform mat4 projectionMatrix;
uniform mat4 mdlMatrix;
uniform mat4 wrlMatrix;

uniform vec3 p; // camera position in model coordinates

in vec3 in_Normal;
out vec3 v_pos_view;
out vec3 v_normal_view;

in vec2 inTexCoord;
out vec2 texCoord;

out vec3 exColor;

uniform float t;

void main(void)
{
	mat3 normalMatrix = transpose(inverse(mat3(wrlMatrix * mdlMatrix)));
	v_normal_view = normalMatrix * in_Normal;
	v_pos_view = vec3(wrlMatrix * mdlMatrix * vec4(in_Position, 1.0));

	vec4 camera_pos_view = wrlMatrix * mdlMatrix * vec4(p, 1.0);
	gl_Position = projectionMatrix * vec4(v_pos_view, 1.0);

	texCoord = inTexCoord;
	
	//exColor = 0.25*(exColor2 + exColor3 + exColor0 + exColor1); // 2 directional lights + 1 positional light
	//exColor = 0.5*(exColor2 + exColor3); // 2 directional lights

	//exColor = v_normal_view; // normal vectors in view coordinates
	//exColor = vec3(v_pos_view); // surface positions in view coordinates
}
