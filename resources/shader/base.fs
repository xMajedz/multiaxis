#version 330

uniform vec3 viewPosition;
uniform vec3 objectColor;
uniform float objectAlpha;

in vec3 fragPosition;
in vec3 fragNormal;

out vec4 finalColor;

void main()
{
    vec3 lightPosition = vec3(0.0, 0.0, 10.0);
    vec3 lightAmbient  = vec3(0.5, 0.5, 0.5);
	vec3 lightDiffuse  = vec3(0.4, 0.4, 0.4);
	vec3 lightSpecular = vec3(0.0, 0.0, 0.0);

    vec3 lightDirection = normalize(fragPosition - lightPosition);

	vec3 objectAmbient  = vec3(0.8, 0.8, 0.8);
	vec3 objectDiffuse  = vec3(0.8, 0.8, 0.8);
	vec3 objectSpecular = vec3(0.0, 0.0, 0.0);
	
	vec3 ambient = lightAmbient * objectAmbient;

    float diff = max(dot(fragNormal, -lightDirection), 0.0);
	vec3 diffuse = diff * lightDiffuse * objectDiffuse;

    vec3 viewDirection = normalize(viewPosition - fragPosition);
	vec3 reflectDirection = reflect(lightDirection, fragNormal);

    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 32);
	vec3 specular = spec * lightSpecular * objectSpecular;

    finalColor = vec4((ambient + diffuse + specular) * objectColor, objectAlpha);
}

