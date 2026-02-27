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
	
	vec3 light2 = lightSourcesDirPosArr[2];
	vec3 color2 = lightSourcesColorArr[2];

	vec3 light3 = lightSourcesDirPosArr[3];
	vec3 color3 = lightSourcesColorArr[3];

	float shade2;
	shade2 = dot(normalize(ex_Normal), light2);
	shade2 = max(0.0, shade2);
	vec3 exColor2 = vec3(shade2);
	exColor2.x = exColor2.x * color2.x;
	exColor2.y = exColor2.y * color2.y;
	exColor2.z = exColor2.z * color2.z;

	float shade3;
	shade3 = dot(normalize(ex_Normal), light3);
	shade3 = max(0.0, shade3);
	vec3 exColor3 = vec3(shade3);
	exColor3.x = exColor3.x * color3.x;
	exColor3.y = exColor3.y * color3.y;
	exColor3.z = exColor3.z * color3.z;

	vec3 light0 = lightSourcesDirPosArr[0];
	vec3 color0 = lightSourcesColorArr[0];
	vec3 local_vector0 = light0 - vec3(surface_pos_view);
	float shade0;
	shade0 = dot(normalize(ex_Normal), normalize(local_vector0));
	shade0 = max(0.0, shade0);
	vec3 exColor0 = vec3(shade0);
	exColor0.x = exColor0.x * color0.x;
	exColor0.y = exColor0.y * color0.y;
	exColor0.z = exColor0.z * color0.z;


	vec3 light1 = lightSourcesDirPosArr[1];
	vec3 color1 = lightSourcesColorArr[1];
	vec3 local_vector1 = light1 - vec3(surface_pos_view);
	float shade1;
	shade1 = dot(normalize(ex_Normal), normalize(local_vector1));
	shade1 = max(0.0, shade1);
	vec3 exColor1 = vec3(shade1);
	exColor1.x = exColor1.x * color1.x;
	exColor1.y = exColor1.y * color1.y;
	exColor1.z = exColor1.z * color1.z;

	

	exColor = 0.25*(exColor2 + exColor3 + exColor0 + exColor1); // 2 directional lights + 1 positional light
	//exColor = 0.5*(exColor2 + exColor3); // 2 directional lights

	//exColor = ex_Normal; // normal vectors in view coordinates
	//exColor = vec3(surface_pos_view); // surface positions in view coordinates
}
