#include "Entity.h"

#include <string>
#include <stdlib.h>
#include <iostream>
#include <algorithm>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#include "../managers/graphics/GraphicsManager.h"

Entity::Entity(Transform* _transform) : transform(_transform) {
	//Set Position
	_transform->SetPosition(glm::vec3(0.0f, -0.5f, 0.0f));

	//Add Fractal Rules
	rules.insert(std::make_pair('X', "F[[F[[[FFX]/-FX]*+FX]-*FX]+/FX]"));
	rules.insert(std::make_pair('F', "FF"));

	//Create Circle Sweep Surface
	for (float u = 0.0f; u < 2 * M_PI; u += 0.5f) {
		sweepSurface.push_back(glm::vec3(0.01f * cos(u), 0.01f * sin(u), 0.0f));
	}

	//Bind VAO, NBO
	glGenVertexArrays(1, &vertexArrayId);
	glBindVertexArray(vertexArrayId);

	glGenBuffers(1, &vertexBufferId);
	glGenBuffers(1, &normalBufferId);
	glGenBuffers(1, &indicesBufferId);

	std::cout << glfwGetTime() << std::endl;
	//Generate Fractals 1-(numberIterations) (~90 Seconds)
	GenerateFractals();
	std::cout << glfwGetTime() << std::endl;

	//Build Fractal Vertices
	LoadFractal();

	//Build Fractal Sweep
	LoadSweep();

	//Calculate Normals
	ComputeNormals();
}

void Entity::Render(GLuint _programId) {
	//Render ImGui
	//Seed
	if (ImGui::SliderInt("Seed", &seed, 0, 5)) {
		LoadFractal();
		LoadSweep();
		ComputeNormals();
	}
	//Fractal Iteration
	if (ImGui::SliderInt("Number of Iterations", &fractalIndex, 1, numberIterations)) {
		LoadFractal();
		LoadSweep();
		ComputeNormals();
	}
	//Object Color
	if (ImGui::ColorEdit3("Color", glm::value_ptr(objectColor)));
	//Object Position
	glm::vec3 position = transform->GetPosition();
	if(ImGui::DragFloat3("Position", glm::value_ptr(position), 0.001f))
		transform->SetPosition(position);
	//Object Scale
	glm::vec3 scale = transform->GetScale();
	if (ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.01f))
		transform->SetScale(scale);

	//Bind VAO
	glBindVertexArray(vertexArrayId);
	glUseProgram(_programId);
	//Vertex Buffer
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);
	//Normal Buffer
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, normalBufferId);
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);
	//Vertex Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBufferId);

	//glDrawArrays(GraphicsManager::GetDrawMode(), 0, vertexBufferData.size());//without indices
	glDrawElements(//with indices
		GraphicsManager::GetDrawMode(),
		vertexIndicesData.size(),
		GL_UNSIGNED_INT,
		(void*)0
	);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

//Return the Objects Color
glm::vec3 Entity::GetObjectColor() {
	return objectColor;
}

//Generates and Caches the First numberIterations Fractals
void Entity::GenerateFractals() {
	fractalPatterns.push_back(start);
	std::string next;
	std::map<char, std::string>::iterator it;
	for (int i = 0; i < numberIterations; i++) {
		next = "";
		for (char c : fractalPatterns[i]) {
			it = rules.find(c);
			if (it != rules.end()) {
				next += rules[c];
			} else {
				next += c;
			}
		}
		fractalPatterns.push_back(next);
	}
	for (int i = 0; i < numberIterations; i++) {
		fractalPatterns[i].erase(std::remove(fractalPatterns[i].begin(), fractalPatterns[i].end(), 'X'), fractalPatterns[i].end());
	}
}

//Load and Cache Fractal Vertices
void Entity::LoadFractal() {
	//Initialize Random Number Generator
	srand(seed);
	//Clear Previous Vertices
	vertexBufferData.clear();
	vertexDirectionData.clear();
	isTip.clear();
	//Fractal Starting Values
	glm::vec3 startPosition = glm::vec3(0.0f);
	glm::vec3 startDirection = Transform::UP;
	//float startDiameter = 0.125f;
	//Fractal Current Values
	glm::vec3 currentPosition = startPosition;
	glm::vec3 currentDirection = startDirection;
	//float currentDiameter = startDiameter;
	//Fractal Constants
	const float lineLength = 0.1f / (float)fractalIndex;
	//Fractal Variables
	std::vector<glm::vec3> returnPositions;
	std::vector<glm::vec3> returnDirections;
	//std::vector<float> returnDiameters;
	//Main Loop
	//for (char c : fractalPatterns[fractalIndex]) {
		//vertexBufferData.push_back(currentPosition);
	vertexBufferData.push_back(currentPosition);
	vertexDirectionData.push_back(currentDirection);
	isTip.push_back(false);
	for (int i = 0; i < fractalPatterns[fractalIndex].size(); i++) {
		char c = fractalPatterns[fractalIndex][i];
		switch (c) {
		case 'F':
			//Move Forward, Caching New Position, and Direction
			currentPosition = currentPosition + (lineLength * currentDirection);
			vertexBufferData.push_back(currentPosition);
			vertexDirectionData.push_back(currentDirection);
			if (fractalPatterns[fractalIndex][i + 1] == ']' || i == fractalPatterns[fractalIndex].size() - 1) {
				isTip.push_back(true);
			} else {
				isTip.push_back(false);
			}
			//diameters.push_back(currentDiameter);
			//currentDiameter *= 0.99f;
			break;
		case '+':
			//Rotate Random Amount in X Direction
			currentDirection = glm::normalize(glm::rotate(((float)rand() / RAND_MAX) * angle, Transform::RIGHT) * glm::vec4(currentDirection, 0.0f)); //glm::normalize(glm::vec3(currentDirection.x + (float)rand() / RAND_MAX, currentDirection.y, currentDirection.z));
			break;
		case '-':
			//Rotate Random Amount in X Direction
			currentDirection = glm::normalize(glm::rotate(((float)rand() / RAND_MAX) * -angle, Transform::RIGHT) * glm::vec4(currentDirection, 0.0f)); //glm::normalize(glm::vec3(currentDirection.x - (float)rand() / RAND_MAX, currentDirection.y, currentDirection.z));
			break;
		case '/':
			//Rotate Random Amount in Z Direction
			currentDirection = glm::normalize(glm::rotate(((float)rand() / RAND_MAX) * -angle, Transform::FORWARD) * glm::vec4(currentDirection, 0.0f)); //glm::normalize(glm::vec3(currentDirection.x, currentDirection.y, currentDirection.z - (float)rand() / RAND_MAX));
			break;
		case '*':
			//Rotate Random Amount in Z Direction
			currentDirection = glm::normalize(glm::rotate(((float)rand() / RAND_MAX) * angle, Transform::FORWARD) * glm::vec4(currentDirection, 0.0f)); //glm::normalize(glm::vec3(currentDirection.x, currentDirection.y, currentDirection.z + (float)rand() / RAND_MAX));
			break;
		case'[':
			//Cache Position and Direction
			returnPositions.push_back(currentPosition);
			returnDirections.push_back(currentDirection);
			//returnDiameters.push_back(currentDiameter);
			break;
		case']':
			//Load Position and Direction
			currentPosition = returnPositions.back();
			returnPositions.pop_back();
			currentDirection = returnDirections.back();
			returnDirections.pop_back();
			vertexBufferData.push_back(currentPosition);
			vertexDirectionData.push_back(currentDirection);
			if (fractalPatterns[fractalIndex][i + 1] == ']' || i == fractalPatterns[fractalIndex].size() - 1) {
				isTip.push_back(true);
			} else {
				isTip.push_back(false);
			}
			//currentDiameter = 0.90f * returnDiameters.back();
			//returnDiameters.pop_back();
			break;
		default:
			break;
		}
	}
}

void Entity::LoadSweep() {
	//Store Fractal Verticies Temporarily
	std::vector<glm::vec3> tmpVertexBufferData = vertexBufferData;
	std::vector<bool> tmpTips = isTip;
	float tipValue = 1.0f;
	//Clear Vertices
	vertexBufferData.clear();
	vertexIndicesData.clear();
	isTip.clear();
	//For Each Temporary Vertex, Add Sweep Surface
	for (int i = 0; i < tmpVertexBufferData.size(); i++) {
		//Push Back Each Vertex From Sweep Surface
		if (tmpTips[i]) {
			tipValue = 0.0001f;
		} else {
			tipValue = 1.0f;
		}
		for (int j = 0; j < sweepSurface.size(); j++) {
			glm::mat4 rotation = glm::inverse(glm::lookAt(glm::vec3(0.0f), vertexDirectionData[i], Transform::UP));
			if (vertexDirectionData[i] == Transform::UP) {
				rotation = glm::inverse(glm::lookAt(glm::vec3(0.0f), vertexDirectionData[i], Transform::FORWARD));
			}
			//Sweep Surface Shifted by Fractal "Center"
			vertexBufferData.push_back(glm::vec3(rotation * glm::vec4(tipValue * sweepSurface[j], 0.0f)) + tmpVertexBufferData[i]);
			isTip.push_back(tmpTips[i]);
		}
	}

	//Fill Indices Buffer
	for (int i = 0; i < vertexBufferData.size() - sweepSurface.size() - 1; i++) {
		if (!isTip[i]) {
			if ((i + 1) % (sweepSurface.size()) == 0) {
				vertexIndicesData.push_back(i);
				vertexIndicesData.push_back(i + sweepSurface.size());
				vertexIndicesData.push_back(i + 1);
				vertexIndicesData.push_back(i);
				vertexIndicesData.push_back(i + 1);
				vertexIndicesData.push_back(i - sweepSurface.size() + 1);
			} else {
				vertexIndicesData.push_back(i);
				vertexIndicesData.push_back(i + sweepSurface.size());
				vertexIndicesData.push_back(i + sweepSurface.size() + 1);
				vertexIndicesData.push_back(i);
				vertexIndicesData.push_back(i + sweepSurface.size() + 1);
				vertexIndicesData.push_back(i + 1);
			}
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertexBufferData.size(), vertexBufferData.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexIndicesData.size() * sizeof(unsigned int), &vertexIndicesData[0], GL_STATIC_DRAW);
}

void Entity::ComputeNormals() {
	//Clear Normal Buffer Data
	normalBufferData.clear();
	//Variables
	glm::vec3 normal, edge1, edge2;
	//For 1 Vertex on Each Face, Calculate Normal
	for (int i = 0; i < vertexBufferData.size() - sweepSurface.size() - 1; i++) {
		//Calculate Normal
		if ((i + 1) % sweepSurface.size() == 0) {
			edge2 = vertexBufferData[i + 1] - vertexBufferData[i];
			edge1 = vertexBufferData[i + sweepSurface.size()] - vertexBufferData[i];
		} else {
			edge2 = vertexBufferData[i + sweepSurface.size() + 1] - vertexBufferData[i];
			edge1 = vertexBufferData[i + sweepSurface.size()] - vertexBufferData[i];
		}
		normal = glm::normalize(glm::cross(edge1, edge2));
		normalBufferData.push_back(normal);
		//for (int j = 0; j < 3; j++) {
		//	//Store Normal in Buffer
		//	normalBufferData.push_back(normal);
		//}
	}

	glBindBuffer(GL_ARRAY_BUFFER, normalBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normalBufferData.size(), normalBufferData.data(), GL_STATIC_DRAW);
}