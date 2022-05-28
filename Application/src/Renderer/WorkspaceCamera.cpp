#include "PCH.h"
#include "WorkspaceCamera.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"

#include "Core/Application.h"

WorkspaceCamera::WorkspaceCamera()
    : m_lastFrameMousePosition(Application::getInput().getMousePosition())
{
    recalculateCameraPosition();
}

void WorkspaceCamera::onUpdate(TimeStep ts)
{
    const Input& input = Application::getInput();
    glm::vec2 currentMousePosition = input.getMousePosition();
    // Don't need to scale by the timestep because that is already done implicitly by getting the position of the mouse each frame
    glm::vec2 mousePositionDelta = (currentMousePosition - m_lastFrameMousePosition);
    m_lastFrameMousePosition = currentMousePosition;

    if (input.isMouseButtonPressed(MouseButtonCode::MOUSE_BUTTON_LEFT))
        panCamera(mousePositionDelta);
    if (input.isMouseButtonPressed(MouseButtonCode::MOUSE_BUTTON_RIGHT))
        rotateCamera(mousePositionDelta);
}

void WorkspaceCamera::onWindowResizeEvent(uint32_t width, uint32_t height)
{
    m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    m_viewportSize = { width, height };
}

void WorkspaceCamera::onMouseScrollEvent(float xOffset, float yOffset)
{
    zoomCamera(-yOffset);
}

glm::mat4 WorkspaceCamera::getViewMatrix() const
{
    return glm::lookAt(m_cameraPosition, m_cameraFocusPoint, WORLD_UP_DIRECTION);
}

glm::mat4 WorkspaceCamera::getProjectionMatrix() const
{
    return glm::perspective(m_vFov, m_aspectRatio, m_nearClip, m_farClip);
}

glm::vec3 WorkspaceCamera::getCameraPosition() const
{
    return m_cameraPosition;
}

void WorkspaceCamera::panCamera(const glm::vec2& mousePositionDelta)
{
    float xPanSpeed = glm::max((PAN_SPEED * (1.0f / static_cast<float>(m_viewportSize.x)) * m_zoomLevel), 0.0005f);
    float yPanSpeed = glm::max((PAN_SPEED * (1.0f / static_cast<float>(m_viewportSize.y)) * m_zoomLevel), 0.0005f);
    m_cameraFocusPoint -= mousePositionDelta.x * getRightDirection() * xPanSpeed;
    m_cameraFocusPoint += mousePositionDelta.y * getUpDirection() * yPanSpeed;

    recalculateCameraPosition();
}

void WorkspaceCamera::rotateCamera(const glm::vec2& mousePositionDelta)
{
    m_yaw   -= mousePositionDelta.x * ROTATE_SPEED;
    m_pitch -= mousePositionDelta.y * ROTATE_SPEED;

    // Prevent gimbal lock

    if (glm::degrees(m_pitch) > 89.0f)
        m_pitch = glm::radians(89.0f);
    else if (glm::degrees(m_pitch) < -89.0f)
        m_pitch = glm::radians(-89.0f);

    recalculateCameraPosition();
}

void WorkspaceCamera::zoomCamera(float delta)
{
    float newZoomLevel = m_zoomLevel + (delta * ZOOM_SPEED * m_zoomLevel);
    m_zoomLevel = glm::max(newZoomLevel, 0.05f);

    recalculateCameraPosition();
}

void WorkspaceCamera::recalculateCameraPosition()
{
    m_cameraPosition = m_cameraFocusPoint - (getForwardDirection() * m_zoomLevel);
}

glm::quat WorkspaceCamera::getOrientation()
{
    return glm::quat(glm::vec3(m_pitch, m_yaw, 0.0f));
}

glm::vec3 WorkspaceCamera::getRightDirection()
{
    return glm::rotate(getOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
}

glm::vec3 WorkspaceCamera::getUpDirection()
{
    return glm::rotate(getOrientation(), WORLD_UP_DIRECTION);
}

glm::vec3 WorkspaceCamera::getForwardDirection()
{
    return glm::rotate(getOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
}
