#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPositionModel;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexNormalModel;

//Output Data
out vec3 vertexPositionWorld;
out vec3 vertexNormalCamera;
out vec3 cameraDirectionCamera;
out vec3 lightDirectionCamera;
out vec3 colour;

//Values for Mesh
uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform vec3 lightPositionWorld;

void main(){

    gl_Position = MVP * vec4(vertexPositionModel, 1.0f);

	vertexPositionWorld = (M * vec4(vertexPositionModel, 1)).xyz;

	vec3 vertexPositionCamera = (V * M * vec4(vertexPositionModel, 1)).xyz;
	cameraDirectionCamera = vec3(0.0f, 0.0f, 0.0f) - vertexPositionCamera;

	vec3 lightPositionCamera = (V * vec4(lightPositionWorld, 1)).xyz;
	lightDirectionCamera = lightPositionCamera + cameraDirectionCamera;

	vertexNormalCamera = mat3(transpose(inverse(V * M))) * vertexNormalModel;

	colour = vertexColor;
}

