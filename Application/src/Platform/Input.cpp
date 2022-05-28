#include "PCH.h"
#include "Input.h"

bool Input::isKeyPressed(KeyCode keyCode) const
{
    ASSERT_MESSAGE(m_glfwWindow, "Cannot poll inputs as m_glfwWindow is a nullptr");

    int32_t action = glfwGetKey(m_glfwWindow, static_cast<int32_t>(keyCode));
    return action == GLFW_PRESS;
}

bool Input::isMouseButtonPressed(MouseButtonCode mousebuttonCode) const
{
    ASSERT_MESSAGE(m_glfwWindow, "Cannot poll inputs as m_glfwWindow is a nullptr");

    int32_t action = glfwGetMouseButton(m_glfwWindow, static_cast<int32_t>(mousebuttonCode));
    return action == GLFW_PRESS;
}

glm::vec2 Input::getMousePosition() const
{
    ASSERT_MESSAGE(m_glfwWindow, "Cannot poll inputs as m_glfwWindow is a nullptr");

    double x, y;
    glfwGetCursorPos(m_glfwWindow, &x, &y);

    return { static_cast<float>(x), static_cast<float>(y) };
}
