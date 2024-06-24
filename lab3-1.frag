#version 150

out vec4 out_Color;
out vec4 out_Color2;

in vec3 o_Color;
in vec2 texCoord;
in vec3 transformedNormal;
in vec3 surfacePos;

uniform vec3 lightSourcesDirPosArr[4];

uniform vec3 lightSourcesColorArr[4];

uniform float specularExponent;

uniform bool isDirectional[4];
uniform mat4 realV;

void main(void)
{
	vec3 total = vec3(0,0,0);
	for(int i=0; i<4; i++){
		if(isDirectional[i]){
			float shade;
			vec3 lightSource = mat3(realV) * lightSourcesDirPosArr[i];
			shade = dot(normalize(lightSource), normalize(transformedNormal));
			shade = clamp(shade, 0, 1);
			vec3 resulting_light = (shade * lightSourcesColorArr[i]);

			vec3 reflectedDirection = normalize(reflect(-lightSource, transformedNormal));
			vec3 eyeDirection = normalize(-surfacePos);
			float specularStrength = 0.0;
			if(dot(lightSource, transformedNormal) > 0.0){
				specularStrength = dot(reflectedDirection, eyeDirection);
				float exponent = 100.0;
				specularStrength = max(specularStrength, 0.01);
				specularStrength = pow(specularStrength, exponent);
				
			}

			vec3 specular = specularStrength * lightSourcesColorArr[i];

			total += (resulting_light*0.5) + specular*0.8;
		}
		else{
			float shade;
			vec3 lightSource = vec3(realV * vec4(lightSourcesDirPosArr[i], 1.0)) - surfacePos;
			shade = dot(normalize(lightSource), normalize(transformedNormal));
			shade = clamp(shade, 0, 1);
			vec3 resulting_light = shade * lightSourcesColorArr[i];
		
			vec3 reflectedDirection = normalize(reflect(-lightSource, transformedNormal));
			vec3 eyeDirection = normalize(-surfacePos);
			float specularStrength = 0.0;
			if(dot(lightSource, transformedNormal) > 0.0){
				specularStrength = dot(reflectedDirection, eyeDirection);
				float exponent = 100.0;
				specularStrength = max(specularStrength, 0.01);
				specularStrength = pow(specularStrength, exponent);
				
			}
			vec3 specular = specularStrength * lightSourcesColorArr[i];


			total += (resulting_light*0.5) + specular*0.8;
		}
	}
	out_Color = vec4(total, 1);
}