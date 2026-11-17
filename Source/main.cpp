#pragma once

// Include standard headers
#include <iostream>

// Include GLEW and GLFW
#include "glew/glew.h"
#include "glfw/glfw3.h"

// Include GLM
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

// Include our helper function for loading shaders
#include "Core/game.hpp"
#include "Core/resource_manager.hpp"

static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
static void deltaTime(float& dt, float& last_frame);

const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;

Game WXW(SCREEN_WIDTH, SCREEN_HEIGHT);

int main(void)
{
	if (!glfwInit())
	{
		std::cerr << "Failed to initiate GLFW.\n";
		return -1;
	}
	else {
		std::cout << "Successfully initiated GLFW.\n";
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	#ifdef __APPLE__
		
    	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif
	glfwWindowHint(GLFW_RESIZABLE, false);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Weapon X: Wolverine", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cerr << "Failed to open GLFW window.\n";
		glfwTerminate();
		return -1;
	} else {
		std::cout << "Successfully opened GLFW window.\n";
	}

	glfwMakeContextCurrent(window);

	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW\n";
		glfwTerminate();
		return -1;
	} else {
		std::cout << "Successfully initialized GLEW.\n";
	}

	// User input: register the callback that records keyboard presses and releases.
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	WXW.init();

	float dt = 0.0f;
	float lastFrame = 0.0f;

	while (!glfwWindowShouldClose(window))
	{
		deltaTime(dt, lastFrame);
		// Dispatch pending keyboard events before processInput reads the saved key states.
		glfwPollEvents();

		// Manage player input
		WXW.processInput(dt);

		// Update game state
		WXW.update(dt);

		// Render
		glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		WXW.render();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);
	} 

	ResourceManager::clear();
	glfwTerminate();
	return 0;
}

static void framebuffer_size_callback(GLFWwindow* window, int new_width, int new_height)
{
	glViewport(0, 0, new_width, new_height);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// When a player presses the escape key, we set the WindowShouldClose property to true, closing the application
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	// Store held/released keys for continuous movement; bounds-check before indexing Keys.
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			WXW.Keys.at(key) = true;
		else if (action == GLFW_RELEASE)
			WXW.Keys.at(key) = false;
	}
}

static void deltaTime(float& dt, float& last_frame) {
	float currentFrame = glfwGetTime();
	dt = currentFrame - last_frame;
	last_frame = currentFrame;
}
