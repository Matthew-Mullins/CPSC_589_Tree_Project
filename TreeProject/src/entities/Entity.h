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

	Transform* transform;
private:
	GLuint vertexArrayId;
	GLuint matrixId;

	GLuint vertexBufferId;
	GLuint colorBufferId;
	GLuint normalBufferId;

	std::vector<glm::vec3> vertexBufferData;
	std::vector<glm::vec3> colorBufferData;
	std::vector<glm::vec3> normalBufferData;

	std::vector<glm::vec3> skeletonBufferData;
	std::vector<glm::vec3> skeletonDirectionData;
	std::vector<glm::vec3> barkBufferData;

	glm::vec3 objectColor;

	std::vector<glm::vec3> sweepSurface;

	int seed = 0;
	int numberIterations = 4;

	std::string variables[2] = {"X", "F"};
	std::string constants[6] = { "/", "*", "-", "+", "[", "]" };
	std::string start = "X";
	std::string skeleton;
	std::map<char, std::string> rules;
	
	void BuildFractal();
	void ReadFractal();
	void BuildBark();

	void UpdateColor();
	void ComputeNormals();
};