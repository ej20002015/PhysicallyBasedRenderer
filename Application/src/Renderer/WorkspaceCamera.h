#pragma once
#include "PCH.h"

#include "Camera.h"

#include "glm/gtc/quaternion.hpp"

class WorkspaceCamera : public Camera
{
public:

	WorkspaceCamera();

	void onUpdate(TimeStep ts);

	void onWindowResizeEvent(uint32_t width, uint32_t height);
	void onMouseScrollEvent(float xOffset, float yOffset);

	glm::mat4 getViewMatrix() const override;
	glm::mat4 getProjectionMatrix() const override;

	glm::vec3 getCameraPosition() const override;

private:

	void panCamera(const glm::vec2& mousePositionDelta);
	void rotateCamera(const glm::vec2& mousePositionDelta);
	void zoomCamera(float delta);

	void recalculateCameraPosition();

	glm::quat getOrientation();
	glm::vec3 getUpDirection();
	glm::vec3 getRightDirection();
	glm::vec3 getForwardDirection();

private:

	inline static const glm::vec3 WORLD_UP_DIRECTION = { 0.0f, 1.0f, 0.0f };
	inline static const float PAN_SPEED = 2.0f;
	inline static const float ROTATE_SPEED = 0.006f;
	inline static const float ZOOM_SPEED = 0.1f;

	glm::vec3 m_cameraPosition;
	glm::vec3 m_cameraFocusPoint = { 0.0f, 0.0f, 0.0f };
	float m_pitch = 0.0f, m_yaw = 0.0f;
	float m_zoomLevel = 5.0f;

	glm::uvec2 m_viewportSize = { DEFAULT_WIDTH, DEFAULT_HEIGHT };
	
	glm::vec2 m_lastFrameMousePosition;

	float m_vFov = 45.0f;
	float m_aspectRatio = DEFAULT_ASPECT_RATIO;
	float m_nearClip = 0.01f, m_farClip = 100.0f;
};