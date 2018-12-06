#pragma once
#include <iostream>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

#include <GL/glew.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "../Manager.h"
#include "../../entities/Entity.h"

struct Camera {
	Camera() {
		distance = 10.0f;
		target = glm::vec3(0.0f);
		position = glm::vec3(0.0f, 0.0f, distance);
		viewMatrix = glm::lookAt(position, target, Transform::UP);
		projectionMatrix = glm::perspective((float)glm::radians(45.0f), 1024.0f / 768.0f, 0.1f, 100.0f);
	}

	void Update() {
		viewMatrix = glm::lookAt(position, target, Transform::UP);
	}

	glm::vec3 target;
	float distance;

	float rotationAngle = 3 * M_PI_2;
	float lift = 0.0f;

	glm::vec3 position;
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
};

class GraphicsManager : public Manager{
public:
	//Singleton Stuff===================================
	static GraphicsManager& GetInstance();
	~GraphicsManager();
	//==================================================

	//Manager Stuff=====================================
	bool Initialize(char* _windowTitle);
	void Update() override;
	//==================================================

	//Window Stuff======================================
	GLFWwindow* GetWindow() const;
	glm::vec2 GetWindowSize() const;
	glm::vec2 GetViewportSize(int _index) const;

	void SetWindowDimensions(int _width, int _height);

	static void WindowSizeCallback(GLFWwindow* window, int width, int height);
	static void ScrollCallback(GLFWwindow* _window, double _x, double _y);
	static void MouseButtonCallback(GLFWwindow* _window, int _button, int _action, int _mods);
	static void CursorPositionCallback(GLFWwindow* _window, double _xPos, double _yPos);
	//==================================================

	static GLenum GetDrawMode();

private:
	const char* VERTEX_FILE_PATH = "src/shaders/vertex.glsl";
	const char* FRAGMENT_FILE_PATH = "src/shaders/fragment.glsl";

	static GLenum drawMode;
	static int currentDrawMode;
	static std::vector<GLenum> drawModes;
	static const char* drawLabels[];

	GLuint programId;
	GLuint matrixId;
	GLuint viewMatrixId;
	GLuint modelMatrixId;
	GLuint lightId;

	Camera* camera;
	std::vector<Entity*> entities;

	//Singleton Stuff===================================
	GraphicsManager();
	GraphicsManager(GraphicsManager const&) = delete;
	void operator=(GraphicsManager const&) = delete;
	//==================================================

	glm::vec2 cursorPos;
	bool middleMouseDown = false;
	bool leftShiftDown = false;

	//Window Stuff======================================
	GLFWwindow* window;
	//Initial Screen Dimensions
	const int WINDOW_WIDTH = 1024;
	const int WINDOW_HEIGHT = 768;
	glm::vec3 backgroundColor = glm::vec3(135.0f / 255.0f, 206.0f / 255.0f, 250.0f / 255.0f);
	int windowWidth;
	int windowHeight;
	//==================================================

	bool LoadShaders(); //Loads Shaders into Program
};