#pragma once
#include "PCH.h"

#include "glm/glm.hpp"

class Camera
{
public:

	virtual glm::mat4 getViewMatrix() const = 0;
	virtual glm::mat4 getProjectionMatrix() const = 0;
	
	virtual glm::vec3 getCameraPosition() const = 0;
};