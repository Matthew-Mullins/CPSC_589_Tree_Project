#version 330 core

//Input
in vec3 vertexPositionWorld;
in vec3 vertexNormalCamera;
in vec3 cameraDirectionCamera;
in vec3 lightDirectionCamera;
in vec3 colour;

// Ouput data
out vec3 color;

//Values for Mesh
uniform vec3 lightPositionWorld;

void main()
{
	vec3 lightColor = vec3(1, 1, 1);
	float lightPower = 50.0f;

	vec3 materialDiffuseColor = colour;
	vec3 materialAmbientColor = vec3(0.4f, 0.4f, 0.4f) * materialDiffuseColor;
	vec3 materialSpecularColor = vec3(0.3f, 0.3f, 0.3f);

	float distance = length(lightPositionWorld - vertexPositionWorld);

	vec3 n = normalize(vertexNormalCamera);
	vec3 l = normalize(lightDirectionCamera);
	float cosTheta = clamp(dot(n, l), 0, 1);

	vec3 E = normalize(cameraDirectionCamera);
	vec3 R = reflect(-l, n);
	float cosAlpha = clamp(dot(E, R), 0, 1);

	color = materialAmbientColor + 
			(materialDiffuseColor * lightColor * lightPower) * (cosTheta / (distance * distance)) + 
			(materialSpecularColor * lightColor * lightPower) * (pow(cosAlpha, 6) / (distance * distance));
}