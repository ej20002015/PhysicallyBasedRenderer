#pragma once
#include "PCH.h"

#include "RendererUtilities.h"
#include "RendererImplementation.h"
#include "Framebuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

class PBRRendererImplementation : public RendererImplementation
{
public:

	PBRRendererImplementation();
	virtual ~PBRRendererImplementation() = default;
	PBRRendererImplementation(const PBRRendererImplementation&) = delete;

	void onWindowResizeEvent(uint32_t width, uint32_t height) override;

	void beginScene(const Camera& camera, const std::vector<Reference<PointLight>>& pointLights) override;
	void endScene(float exposureLevel = 1.0f) override;

	void drawScene(Reference<Scene> scene, const Camera& camera) override;
	void drawModel(Reference<Model> model, const glm::mat4& transform) override;

private:

	void initialiseHDRMultisampleFramebuffer();
	void initialiseIntermediateHDRFramebuffer();
	void initialiseDefaultMaterialTextures();
	void initialiseQuadBuffers();

	void setLightUniforms(const std::vector<Reference<PointLight>>& pointLights);
	void setMaterialUniforms(const PBRMaterial& material);

private:

	Unique<Framebuffer> m_multisampleHDRFramebuffer;
	Reference<Framebuffer> m_intermediateHDRFramebuffer;
	Unique<Shader> m_PBRShader;
	Unique<Shader> m_postProcessingShader;

	Unique<Texture> m_defaultBaseColorMapTexture;
	Unique<Texture> m_defaultRoughnessMapTexture;
	Unique<Texture> m_defaultMetalnessMapTexture;

	Unique<VertexBuffer> m_quadVertexBuffer;
	Unique<IndexBuffer> m_quadIndexBuffer;
};