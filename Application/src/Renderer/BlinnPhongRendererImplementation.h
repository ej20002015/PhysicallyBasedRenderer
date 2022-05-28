#pragma once
#include "PCH.h"

#include "RendererUtilities.h"
#include "RendererImplementation.h"
#include "Framebuffer.h"
#include "Shader.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Material.h"

class BlinnPhongRendererImplementation : public RendererImplementation
{
public:

	BlinnPhongRendererImplementation();
	virtual ~BlinnPhongRendererImplementation() = default;
	BlinnPhongRendererImplementation(const BlinnPhongRendererImplementation&) = delete;

	void onWindowResizeEvent(uint32_t width, uint32_t height) override;

	void beginScene(const Camera& camera, const std::vector<Reference<PointLight>>& pointLights) override;
	void endScene(float exposureLevel = 1.0f) override;

	void drawScene(Reference<Scene> scene, const Camera& camera) override;
	void drawModel(Reference<Model> model, const glm::mat4& transform) override;

private:

	void initialiseMultisampleFramebuffer();
	void initialiseDefaultMaterialTextures();

	void setLightUniforms(const std::vector<Reference<PointLight>>& pointLights);
	void setMaterialUniforms(const BlinnPhongMaterial& material);

private:

	Unique<Framebuffer> m_multisampleFramebuffer;
	Unique<Shader> m_blinnPhongShader;

	// Default texture maps

	Unique<Texture> m_defaultDiffuseMapTexture;
	Unique<Texture> m_defaultSpecularMapTexture;
};