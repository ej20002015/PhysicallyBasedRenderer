#pragma once
#include "PCH.h"

#include "Camera.h"
#include "Scene/PointLight.h"
#include "Scene/Model.h"
#include "Scene/Scene.h"

class RendererImplementation
{
public:

	virtual ~RendererImplementation() = default;

	virtual void onWindowResizeEvent(uint32_t width, uint32_t height) = 0;

	virtual void drawScene(Reference<Scene> scene, const Camera& camera) = 0;

	virtual void beginScene(const Camera& camera, const std::vector<Reference<PointLight>>& pointLights) = 0;
	virtual void endScene(float exposureLevel = 1.0f) = 0;

	virtual void drawModel(Reference<Model> model, const glm::mat4& transform) = 0;
};