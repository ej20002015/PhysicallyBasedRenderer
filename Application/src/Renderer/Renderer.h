#pragma once
#include "PCH.h"

#include "RendererUtilities.h"
#include "RendererImplementation.h"
#include "BlinnPhongRendererImplementation.h"
#include "PBRRendererImplementation.h"

class Renderer
{
public:

	enum class RendererType
	{
		BLINN_PHONG = 0,
		PBR
	};

public:

	static void init();
	static void shutdown();

	static void onWindowResizeEvent(uint32_t width, uint32_t height);

	static void setRendererType(RendererType rendererType);

	// Drawing methods

	static void beginScene(const Camera& camera, const std::vector<Reference<PointLight>>& pointLights);
	static void endScene(float exposureLevel = 1.0f);

	static void drawScene(const Reference<Scene>& scene, const Camera& camera);
	static void drawModel(Reference<Model> model, const glm::mat4& transform);

	// Utility methods

	static void clear();

	static void setClearColor(const glm::vec4& color);

	static void bindDefaultFramebuffer();

private:

	static RendererID s_vertexArrayRendererID;

	static RendererType s_currentRendererType;

	static RendererImplementation* s_currentRendererImplementation;
	static Unique<BlinnPhongRendererImplementation> s_blinnPhongRendererImplementation;
	static Unique<PBRRendererImplementation> s_PBRRendererImplementation;
};