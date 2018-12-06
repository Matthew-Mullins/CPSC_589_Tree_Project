#pragma once
#include <vector>
#include <map>

#include <GL/glew.h>

#include "../../../include/imgui/imgui.h"
#include "../../../include/imgui/imgui_impl_glfw.h"
#include "../../../include/imgui/imgui_impl_opengl3.h"

#include "Transform.h"

class Entity {
public:
	Entity(Transform* _transform);
	void Render(GLuint _programId);
	glm::vec3 GetObjectColor();
	Transform* transform;
private:
	GLuint vertexArrayId;
	GLuint vertexBufferId;
	GLuint normalBufferId;
	GLuint matrixId;
	GLuint indicesBufferId;

	std::vector<glm::vec3> vertexBufferData;
	std::vector<glm::vec3> normalBufferData;
	std::vector<glm::vec3> vertexDirectionData;
	std::vector<unsigned int> vertexIndicesData;
	std::vector<glm::vec3> sweepSurface;

	int seed = 0;
	int numberIterations = 7;
	int fractalIndex = 4;
	glm::vec3 objectColor = glm::vec3(0.0f, 0.5f, 0.0f);

	//std::vector<float> diameters;
	std::string variables[2] = {"X", "F"};
	std::string constants[6] = { "/", "*", "-", "+", "[", "]" };
	std::string start = "X";
	std::string skeleton;
	std::map<char, std::string> rules;
	std::vector<std::string> fractalPatterns;
	
	void GenerateFractals();
	void LoadFractal();
	void LoadSweep();
	void ComputeNormals();
};