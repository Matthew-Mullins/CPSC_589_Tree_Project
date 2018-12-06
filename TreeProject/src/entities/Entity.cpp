#include "Entity.h"

#include <string>
#include <stdlib.h>
#include <iostream>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#include "../managers/graphics/GraphicsManager.h"

Entity::Entity(Transform* _transform) : transform(_transform) {
	
	//Set Position
	_transform->SetPosition(glm::vec3(0.0f, -0.5f, 0.0f));

	//Add Rules
	//rules.insert(std::make_pair('X', "F[FX[FX[FX[/-FX]*+FX]-*FX]+/FX]"));//good
	rules.insert(std::make_pair('X', "F[[F[[[FFX]//--FX]*/++FX]--**FX]++//FX]"));//good
	rules.insert(std::make_pair('F', "FF"));
	
	//Create Circle Sweep Surface
	for (float u = 0.0f; u < 2 * M_PI; u += 0.5f) {
		sweepSurface.push_back(glm::vec3(0.01f * cos(u), 0.0f, 0.01f * sin(u)));
	}

	//Push Back Colors
	objectColor = glm::vec3(0.0f, 0.5f, 0.0f);

	//Build Tree Fractal
	BuildFractal();

	//UpdateColor();

	//Push Back Normals
	//ComputeNormals();

	glGenVertexArrays(1, &vertexArrayId);
	glBindVertexArray(vertexArrayId);

	glGenBuffers(1, &vertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * skeletonBufferData.size(), skeletonBufferData.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &colorBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * colorBufferData.size(), colorBufferData.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &normalBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, normalBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normalBufferData.size(), normalBufferData.data(), GL_STATIC_DRAW);
}

void Entity::Render(GLuint _programId) {

	if (ImGui::SliderInt("Seed", &seed, 0, 5)) {
		ReadFractal();
	}

	if (ImGui::SliderInt("Number of Iterations", &numberIterations, 1, 6)) {
		BuildFractal();
	}

	if (ImGui::ColorEdit3("Color", glm::value_ptr(objectColor)))
		UpdateColor();

	glm::vec3 position = transform->GetPosition();
	if(ImGui::DragFloat3("Position", glm::value_ptr(position), 0.001f))
		transform->SetPosition(position);

	glm::vec3 scale = transform->GetScale();
	if (ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.01f))
		transform->SetScale(scale);

	glBindVertexArray(vertexArrayId);
	glUseProgram(_programId);

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

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferId);
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalBufferId);
	glVertexAttribPointer(
		2,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glDrawArrays(GraphicsManager::GetDrawMode(), 0, skeletonBufferData.size());
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

void Entity::UpdateColor() {
	colorBufferData.clear();
	for (int i = 0; i < skeletonBufferData.size(); i++) {
		colorBufferData.push_back(objectColor);
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * colorBufferData.size(), colorBufferData.data(), GL_STATIC_DRAW);
}

void Entity::ComputeNormals() {
	normalBufferData.clear();
	for (int i = 0; i < skeletonBufferData.size() - 2; i+=3) {
		glm::vec3 normal;
		glm::vec3 edge1, edge2;
		edge2 = skeletonBufferData[i + 1] - skeletonBufferData[i];
		edge1 = skeletonBufferData[i + 2] - skeletonBufferData[i];
		normal = glm::normalize(glm::cross(edge1, edge2));
		for (int j = 0; j < 3; j++) {
			normalBufferData.push_back(normal);
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, normalBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normalBufferData.size(), normalBufferData.data(), GL_STATIC_DRAW);
}

void Entity::BuildFractal() {
	std::string current = start;
	std::string next;
	std::map<char, std::string>::iterator it;
	for (int i = 0; i < numberIterations; i++) {
		next = "";
		for (char c : current) {
			it = rules.find(c);
			if (it != rules.end()) {
				next += rules[c];
			} else {
				next += c;
			}
		}
		current = next;
	}
	skeleton = next;
	ReadFractal();
}

void Entity::ReadFractal() {
	srand(seed);
	std::cout << rand() % 100 / 100 << std::endl;
	skeletonBufferData.clear();

	glm::vec3 startPos = glm::vec3(0.0f);
	glm::vec3 curPos = startPos;
	glm::vec3 nextPos;
	glm::vec3 startDir = Transform::UP;
	glm::vec3 curDir = startDir;
	float lineLength = 0.02f;
	std::vector<glm::vec3> returnPoints;
	std::vector<glm::vec3> returnAngles;
	for (char c : skeleton) {
		skeletonBufferData.push_back(curPos);
		switch (c) {
		case 'X':
			break;
		case 'F':
			nextPos = curPos + (lineLength * curDir);
			curPos = nextPos;
			skeletonBufferData.push_back(curPos);
			skeletonDirectionData.push_back(curDir);
			break;
		case '+':
			curDir = glm::normalize(glm::vec3(curDir.x + (float)(rand() % 10) / 10.0f, curDir.y, curDir.z));
			break;
		case '-':
			curDir = glm::normalize(glm::vec3(curDir.x - (float)(rand() % 10) / 10.0f, curDir.y, curDir.z));
			break;
		case '/':
			curDir = glm::normalize(glm::vec3(curDir.x, curDir.y, curDir.z + (float)(rand() % 10) / 10.0f));
			break;
		case '*':
			curDir = glm::normalize(glm::vec3(curDir.x, curDir.y, curDir.z - (float)(rand() % 10) / 10.0f));
			break;
		case'[':
			returnPoints.push_back(curPos);
			returnAngles.push_back(curDir);
			break;
		case']':
			curPos = returnPoints.back();
			returnPoints.pop_back();
			curDir = returnAngles.back();
			returnAngles.pop_back();
			break;
		default:
			break;
		}
	}

	BuildBark();	
}

void Entity::BuildBark() {
	std::vector<glm::vec3> tmpBufferData = skeletonBufferData;
	//Sweep Surface
	skeletonBufferData.clear();
	for (int i = 0; i < tmpBufferData.size(); i++) {
		for (int j = 0; j < sweepSurface.size(); j++) {
			skeletonBufferData.push_back(((1.0f - (float)(i / (tmpBufferData.size()/2.0f))) * (sweepSurface[j])) + tmpBufferData[i]);
		}
	}

	tmpBufferData = skeletonBufferData;
	skeletonBufferData.clear();
	for (int i = 0; i < tmpBufferData.size() - sweepSurface.size() - 1; i++) {
		skeletonBufferData.push_back(tmpBufferData[i]);
		skeletonBufferData.push_back(tmpBufferData[i + sweepSurface.size()]);
		skeletonBufferData.push_back(tmpBufferData[i + sweepSurface.size() + 1]);
		skeletonBufferData.push_back(tmpBufferData[i + sweepSurface.size() + 1]);
		skeletonBufferData.push_back(tmpBufferData[i + 1]);
		skeletonBufferData.push_back(tmpBufferData[i]);
	}

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * skeletonBufferData.size(), skeletonBufferData.data(), GL_STATIC_DRAW);
	UpdateColor();
	ComputeNormals();
}