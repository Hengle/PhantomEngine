#version 410 core

layout (location = 0) out vec4 color;

layout( std140) uniform Light
{
    vec4 lightPos;
	vec4 lightColor;
	vec4 lightDir;
} light;

uniform sampler2D diffuseColor;
uniform sampler2D shadowMap;


in vec4 pos;
in vec3 normal;
in vec2 texc;
in vec4 ShadowCoord;

void main()
{
	float ambientStrength = 0.1f;
    vec4 ambient = ambientStrength * light.lightColor;

	vec4 diff = texture(diffuseColor, texc.xy);

	vec3 norm = normalize(normal);
	vec3 dir = normalize(light.lightDir.xyz);
	float diffFactor = max(dot(norm, dir), 0.0);
	vec4 diffuse = diffFactor * light.lightColor;

	float depth = texture( shadowMap, ShadowCoord.xy ).z;

	color = diff * (diffuse + ambient);
} 

