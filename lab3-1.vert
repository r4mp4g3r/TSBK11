#version 150

in vec3 in_Position;
in vec3 in_Color;
in vec3 in_Normal;

uniform mat4 projMatrix;
uniform mat4 realV;
uniform mat4 rotMatrix;

uniform mat4 mdlMatrix;
out vec3 o_Color;
out vec3 transformedNormal;
out vec3 surfacePos;

void main(void)
{
	gl_Position = projMatrix * realV  * mdlMatrix * vec4(in_Position, 1.0);
	mat3 normalMatrix1 = mat3(transpose(inverse(realV * mdlMatrix)));
	transformedNormal = normalMatrix1 *  in_Normal;
	surfacePos = vec3(realV * mdlMatrix * vec4(in_Position, 1.0));
	o_Color = transformedNormal;
}
