#version 150

in vec3 in_Position;
in vec2 inTexCoord;

uniform mat4 projMatrix;
uniform mat4 realV;


uniform sampler2D texunit;


out vec2 texCoord;


void main(void)
{

	gl_Position = projMatrix * realV  * vec4(in_Position, 1.0);

	texCoord = inTexCoord;

}
