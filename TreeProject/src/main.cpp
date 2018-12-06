#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "../include/imgui/imgui.h"
#include "../include/imgui/imgui_impl_glfw.h"
#include "../include/imgui/imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "managers/Manager.h"
#include "managers/application/ApplicationManager.h"
#include "managers/graphics/GraphicsManager.h"
#include "managers/input/InputManager.h"

//CTRL + K, CTRL + C - Comment
//CTRL + K, CTRL + U - UnComment

int main(int argc, char* argv[]) {
	//Vector of Managers
	std::vector<Manager*> managers;

	//Initialize Managers
	//Graphics Manager
	GraphicsManager& graphicsManager = GraphicsManager::GetInstance();
	if (!graphicsManager.Initialize("Window Title")) {
		std::cerr << "Error Initializing the Graphics Manager" << std::endl;
		system("pause");
		return -1;
	}

	//Input Manager
	//InputManager& inputManager = InputManager::GetInstance();
	//inputManager.Initialize();

	//Application Manager
	//ApplicationManager& applicationManager = ApplicationManager::GetInstance();
	//applicationManager.Initialize();

	//Add Systems to Vector
	//managers.push_back(&inputManager);
	//managers.push_back(&applicationManager);
	managers.push_back(&graphicsManager);

	//Application Loop
	while (!glfwWindowShouldClose(graphicsManager.GetWindow())) {
		//Iterate Managers and Update
		for (Manager* manager : managers) {
			manager->Update();
		}
	}
	return 0;
}