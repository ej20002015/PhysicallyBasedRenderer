#pragma once
#include "PCH.h"

#include "glfw3.h"
#include "glm/glm.hpp"

#include "InputCodes.h"

class Window;

class Input
{
public:

	bool isKeyPressed(KeyCode keyCode) const;
	bool isMouseButtonPressed(MouseButtonCode mousebuttonCode) const;

	glm::vec2 getMousePosition() const;

private:

	Input() = default;

	GLFWwindow* m_glfwWindow;

	friend class Window;
};