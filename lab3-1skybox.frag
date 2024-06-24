#version 150

out vec4 out_Color;

in vec2 texCoord;


uniform sampler2D texunit;

void main(void)
{
	out_Color = texture(texunit, texCoord);

	}