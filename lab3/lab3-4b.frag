#version 150

out vec4 out_Color;
in vec2 texCoord;
uniform sampler2D texUnit;
in vec3 exColor;
uniform int has_texture;

in vec3 v_normal_view; // Normal vectors in view coords
in vec3 v_pos_view; // Surface position in view coords

uniform mat4 wrlMatrix;
uniform vec3 lightSourcesDirPosArr[4];
uniform vec3 lightSourcesColorArr[4];
uniform float specularExponent;
uniform bool isDirectional[4];

void main(void)
{
	vec3 N = normalize(v_normal_view); // Lab-PM: Normalize what needs to be normalized.
	// Remember that a varying vector doesn’t stay normalized under interpolation.

	// L2 is normalized surface -> light vector, "s" vector in pfnp page 69
	// - sign here flips direction?
    vec3 L2 = vec3(wrlMatrix * vec4(lightSourcesDirPosArr[2], 0.0));
	// surface -> viewer
	vec3 V2 = normalize(-v_pos_view);
	L2 = normalize(L2);
	// vector r, mirrored around normal
	vec3 R2 = normalize(reflect(-L2, N));
	// sida 70 i pfnp
    float diff2 = max(dot(N, L2), 0.0);
	float spec2 = pow(max(dot(R2, V2), 0.0), specularExponent);
    vec3 color2 = (diff2 + spec2) * lightSourcesColorArr[2]; // blue diffuse
	
    vec3 L3 = vec3(wrlMatrix * vec4(lightSourcesDirPosArr[3], 0.0));
	L3 = normalize(L3);
	vec3 V3 = normalize(-v_pos_view);
	vec3 R3 = normalize(reflect(-L3, N));
    float diff3 = max(dot(N, L3), 0.0);
	float spec3 = pow(max(dot(R3, V3), 0.0), specularExponent);
    vec3 color3 = (diff3 + spec3) * lightSourcesColorArr[3]; // white diffuse

	// world -> view position "light sources are given in world coordinates"
	vec3 light0_view = vec3(wrlMatrix * vec4(lightSourcesDirPosArr[0], 1.0));
	// "Local vector from the surface to the light source"
	vec3 L0 = normalize(light0_view - v_pos_view);
	vec3 V0 = normalize(-v_pos_view);
	vec3 R0 = normalize(reflect(-L0, N));
	float diff0 = max(dot(N, L0), 0.0);
	float spec0 = pow(max(dot(R0, V0), 0.0), specularExponent);
	vec3 color0 = (spec0 + diff0) * lightSourcesColorArr[0];

	vec3 light1_view = vec3(wrlMatrix * vec4(lightSourcesDirPosArr[1], 1.0));
	vec3 L1 = normalize(light1_view - v_pos_view);
	vec3 V1 = normalize(-v_pos_view);
	vec3 R1 = normalize(reflect(-L1, N));
	float diff1 = max(dot(N, L1), 0.0);
	float spec1 = pow(max(dot(R1, V1), 0.0), specularExponent);
	vec3 color1 = (spec1 + diff1) * lightSourcesColorArr[1];

	vec3 color = 0.25 * (color0 + color1 + color2 + color3);
    out_Color = vec4(color, 1.0);

	if(has_texture == 1) {
		vec4 color;
		color = texture(texUnit, texCoord);
		out_Color = color;
	}
}

