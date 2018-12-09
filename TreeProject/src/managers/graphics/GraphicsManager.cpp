#include <stdio.h>
#include <string>
#include <fstream>
#include <algorithm>
#include <sstream>

#include "../../../include/imgui/imgui.h"
#include "../../../include/imgui/imgui_impl_glfw.h"
#include "../../../include/imgui/imgui_impl_opengl3.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "GraphicsManager.h"

GLenum GraphicsManager::drawMode = GL_TRIANGLES;
std::vector<GLenum> GraphicsManager::drawModes = {GL_TRIANGLES, GL_LINE_STRIP};
int GraphicsManager::currentDrawMode = 0;
const char* GraphicsManager::drawLabels[] = {
	"Triangles",
	"Wire Frame"
};

//Singleton
GraphicsManager::GraphicsManager() {}

GraphicsManager& GraphicsManager::GetInstance() {
	static GraphicsManager instance;
	return instance;
}

GraphicsManager::~GraphicsManager() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}

bool GraphicsManager::Initialize(char* _windowTitle) {
	//Initialize GLFW
	if (!glfwInit()) {
		std::cerr << "Error Initializing GLFW" << std::endl;
		system("pause");
		return false;
	}

	//Create Window
	glfwWindowHint(GLFW_SAMPLES, 4); //4x Anti-Aliasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_STENCIL_BITS, 8); //Not Sure if This is Needed or Not
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, _windowTitle, NULL, NULL);
	if (window == NULL) {
		std::cerr << "Error Creating Window" << std::endl;
		system("pause");
		glfwTerminate();
		return false;
	}

	//GLFW Settings
	glfwMakeContextCurrent(window);
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glfwSwapInterval(1); //Double Buffering
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//Initialize ImGui
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfw_InitForOpenGL(window, false);
	ImGui_ImplOpenGL3_Init("#version 130");
	ImGui::StyleColorsDark();
	
	//Input Callbacks
	glfwSetMouseButtonCallback(window, GraphicsManager::MouseButtonCallback);
	glfwSetCursorPosCallback(window, GraphicsManager::CursorPositionCallback);
	glfwSetScrollCallback(window, GraphicsManager::ScrollCallback);
	//glfwSetKeyCallback(window, KeyCallback);
	glfwSetWindowSizeCallback(window, GraphicsManager::WindowSizeCallback);

	//Center Window
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	int xPos = (mode->width - WINDOW_WIDTH) / 2;
	int yPos = (mode->height - WINDOW_HEIGHT) / 2;
	glfwSetWindowPos(window, xPos, yPos);

	//Initialize GL
	glfwGetWindowSize(window, &width, &height);
	windowWidth = width;
	windowHeight = height;

	//Sky Color
	glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.0f);

	//Initialize GLEW
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		std::cout << "Error Initializing GLEW" << std::endl;
		system("pause");
		return false;
	}

	//Load Shaders
	if (!LoadShaders()) {
		std::cout << "Error Loading Shaders" << std::endl;
		system("pause");
		return false;
	}
	matrixId = glGetUniformLocation(programId, "MVP");
	viewMatrixId = glGetUniformLocation(programId, "V");
	modelMatrixId = glGetUniformLocation(programId, "M");
	lightId = glGetUniformLocation(programId, "lightPositionWorld");
	objectColorId = glGetUniformLocation(programId, "objectColor");

	//Set Up Camera
	camera = new Camera();

	//Add Entities
	Entity* entity = new Entity(new Transform());
	entities.push_back(entity);

	return true;
}

void GraphicsManager::Update() {
	glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(programId);

	//ImGui Frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::Begin("Settings");

	//Draw Mode
	if (ImGui::Combo("Render Mode", &GraphicsManager::currentDrawMode, GraphicsManager::drawLabels, IM_ARRAYSIZE(GraphicsManager::drawLabels)))
		GraphicsManager::drawMode = GraphicsManager::drawModes[GraphicsManager::currentDrawMode];

	//Render Entities
	for (Entity* e : entities) {
		e->Render(programId);
		glm::mat4 MVP = camera->projectionMatrix * camera->viewMatrix * e->transform->GetTransformationMatrix();
		glUniformMatrix4fv(matrixId, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(viewMatrixId, 1, GL_FALSE, &camera->viewMatrix[0][0]);
		glUniformMatrix4fv(modelMatrixId, 1, GL_FALSE, &e->transform->GetTransformationMatrix()[0][0]);
		glUniform3f(lightId, -5.0f, 5.0f, 5.0f);
		glm::vec3 objectColor = e->GetObjectColor();
		glUniform3f(objectColorId, objectColor.x, objectColor.y, objectColor.z);
	}

	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Swap buffers
	glfwSwapBuffers(window);
	glfwPollEvents();
}

GLFWwindow* GraphicsManager::GetWindow() const{
	return window;
}

glm::vec2 GraphicsManager::GetWindowSize() const{
	return glm::vec2(windowWidth, windowHeight);
}

glm::vec2 GraphicsManager::GetViewportSize(int _index) const {
	return glm::vec2();
}

void GraphicsManager::SetWindowDimensions(int _width, int _height) {
	windowWidth = _width;
	windowHeight = _height;
}

void GraphicsManager::WindowSizeCallback(GLFWwindow* _window, int _width, int _height) {
	GetInstance().SetWindowDimensions(_width, _height);
}

bool GraphicsManager::LoadShaders() {
	//Create the Shaders
	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

	//Read the Vertex Shader Code from File
	std::string vertexShaderCode;
	std::ifstream vertexShaderStream(VERTEX_FILE_PATH, std::ios::in);
	if (vertexShaderStream.is_open()) {
		std::stringstream sStr;
		sStr << vertexShaderStream.rdbuf();
		vertexShaderCode = sStr.str();
		vertexShaderStream.close();
	} else {
		std::printf("Could Not Open File %s\n", VERTEX_FILE_PATH);
		return false;
	}

	//Read the Fragment Shader Code from File
	std::string fragmentShaderCode;
	std::ifstream fragmentShaderStream(FRAGMENT_FILE_PATH, std::ios::in);
	if (fragmentShaderStream.is_open()) {
		std::stringstream sStr;
		sStr << fragmentShaderStream.rdbuf();
		fragmentShaderCode = sStr.str();
		fragmentShaderStream.close();
	} else {
		std::printf("Could Not Open File %s\n", FRAGMENT_FILE_PATH);
		return false;
	}

	GLint result = GL_FALSE;
	int infoLogLength;

	//Compile Vertex Shader
	std::printf("Compiling Vertex Shader: %s\n", VERTEX_FILE_PATH);
	const char* vertexSourcePointer = vertexShaderCode.c_str();
	glShaderSource(vertexShaderId, 1, &vertexSourcePointer, NULL);
	glCompileShader(vertexShaderId);
	//Check Vertex Shader
	glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vertexShaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0) {
		std::vector<char> vertexShaderErrorMessage(infoLogLength + 1);
		glGetShaderInfoLog(vertexShaderId, infoLogLength, NULL, &vertexShaderErrorMessage[0]);
		std::printf("%s\n", &vertexShaderErrorMessage[0]);
	}

	//Compile Fragment Shader
	std::printf("Compiling Fragment Shader: %s\n", FRAGMENT_FILE_PATH);
	const char* fragmentSourcePointer = fragmentShaderCode.c_str();
	glShaderSource(fragmentShaderId, 1, &fragmentSourcePointer, NULL);
	glCompileShader(fragmentShaderId);
	//Check Fragment Shader
	glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &result);
	glGetShaderiv(fragmentShaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0) {
		std::vector<char> fragmentShaderErrorMessage(infoLogLength + 1);
		glGetShaderInfoLog(fragmentShaderId, infoLogLength, NULL, &fragmentShaderErrorMessage[0]);
		std::printf("%s\n", &fragmentShaderErrorMessage[0]);
	}

	//Link the Program
	std::printf("Linking the Program\n");
	programId = glCreateProgram();
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);
	glLinkProgram(programId);
	// Check the program
	glGetProgramiv(programId, GL_LINK_STATUS, &result);
	glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0) {
		std::vector<char> programErrorMessage(infoLogLength + 1);
		glGetProgramInfoLog(programId, infoLogLength, NULL, &programErrorMessage[0]);
		printf("%s\n", &programErrorMessage[0]);
	}

	glDetachShader(programId, vertexShaderId);
	glDetachShader(programId, fragmentShaderId);

	glDeleteShader(vertexShaderId);
	glDeleteShader(fragmentShaderId);

	return true;
}

GLenum GraphicsManager::GetDrawMode() {
	return drawMode;
}

void GraphicsManager::ScrollCallback(GLFWwindow* _window, double _x, double _y) {
	Camera* camera = GraphicsManager::GetInstance().camera;
	float distance = glm::clamp((float)(camera->distance + _y * -1.0f * 0.4f), 0.5f, 90.0f);
	camera->distance = distance;
	camera->position = distance * glm::normalize(glm::vec3(camera->position.x, camera->position.y, camera->position.z));
	camera->Update();
}

void GraphicsManager::MouseButtonCallback(GLFWwindow* _window, int _button, int _action, int _mods) {
	if (_button == GLFW_MOUSE_BUTTON_MIDDLE){
		if (_action == GLFW_PRESS) {
			GraphicsManager::GetInstance().middleMouseDown = true;
			double x, y;
			glfwGetCursorPos(_window, &x, &y);
			GraphicsManager::GetInstance().cursorPos = glm::vec2(x, y);
		} else if (_action == GLFW_RELEASE) {
			GraphicsManager::GetInstance().middleMouseDown = false;
		}
	}
	if (_button == GLFW_KEY_LEFT_SHIFT) {
		if (_action == GLFW_PRESS) {
			GraphicsManager::GetInstance().leftShiftDown = true;
		} else if (_action == GLFW_RELEASE) {
			GraphicsManager::GetInstance().leftShiftDown = false;
		}
	}
}

void GraphicsManager::CursorPositionCallback(GLFWwindow* _window, double _xPos, double _yPos) {
	if (GraphicsManager::GetInstance().middleMouseDown) {
		if (!GraphicsManager::GetInstance().leftShiftDown) {
			glm::vec2 oldCursor = GraphicsManager::GetInstance().cursorPos;
			Camera* camera = GraphicsManager::GetInstance().camera;
			float rotAngle = camera->rotationAngle + 0.01f * (oldCursor.x - _xPos);
			float tmpLift = camera->lift - 0.01f * (oldCursor.y - _yPos);
			glm::vec3 newPos = camera->distance * glm::normalize(glm::vec3(cos(rotAngle),
				tmpLift,
				-sin(rotAngle))
				- camera->target);
			GraphicsManager::GetInstance().cursorPos = glm::vec2(_xPos, _yPos);
			camera->rotationAngle = rotAngle;
			camera->lift = tmpLift;
			camera->position = newPos;
			camera->Update();
		}
	}
}