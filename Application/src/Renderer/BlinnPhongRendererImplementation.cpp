#include "PCH.h"
#include "BlinnPhongRendererImplementation.h"

#include "VertexBufferLayout.h"

BlinnPhongRendererImplementation::BlinnPhongRendererImplementation()
{
	initialiseMultisampleFramebuffer();

	m_blinnPhongShader = createUnique<Shader>(std::initializer_list<std::string>
	{
		"Assets/Shaders/BlinnPhong.glsl.vert",
		"Assets/Shaders/BlinnPhong.glsl.frag"
	});

	initialiseDefaultMaterialTextures();

	Log::info("Blinn-Phong renderer initialised");
}

void BlinnPhongRendererImplementation::onWindowResizeEvent(uint32_t width, uint32_t height)
{
	m_multisampleFramebuffer->onWindowResizeEvent(width, height);
}

void BlinnPhongRendererImplementation::beginScene(const Camera& camera, const std::vector<Reference<PointLight>>& pointLights)
{
	Log::trace("Beginning to render a Blinn-Phong scene");

	m_multisampleFramebuffer->bind();
	m_multisampleFramebuffer->clear();

	m_blinnPhongShader->bind();

	m_blinnPhongShader->setUniformToValue("u_projectionViewMatrix", camera.getProjectionMatrix() * camera.getViewMatrix());
	m_blinnPhongShader->setUniformToValue("u_viewPosition", camera.getCameraPosition());

	setLightUniforms(pointLights);
}

void BlinnPhongRendererImplementation::endScene(float exposureLevel)
{
	// Blit contents of m_framebuffer to the default framebuffer so it appears in the window
	m_multisampleFramebuffer->blitToTargetFramebuffer();

	Log::trace("Ended the rendering of a Blinn-Phong scene");

	// exposureLevel is not used in the BlinnPhong renderer but is still passed in to keep the API consistent
}

void BlinnPhongRendererImplementation::drawScene(Reference<Scene> scene, const Camera& camera)
{
	Log::trace("Drawing Blinn-Phong scene");

	beginScene(camera, scene->getPointLights());

	for (auto modelAndTransform : scene->getModelsAndTransforms())
		drawModel(modelAndTransform.first, modelAndTransform.second);

	endScene();
}

void BlinnPhongRendererImplementation::drawModel(Reference<Model> model, const glm::mat4& transform)
{
	Log::trace("Drawing Blinn-Phong model {0} with {1} meshes", model->getModelIdentifier(), static_cast<uint32_t>(model->getMeshes().size()));

	model->getVertexBuffer()->bind();
	model->getIndexBuffer()->bind();

	const std::vector<Reference<Material>>& materials = model->getMaterials();
	const auto& materialToMeshMapping = model->getMaterialToMeshMapping();
	const std::vector<Model::Mesh>& meshes = model->getMeshes();

	for (uint32_t i = 0; i < materials.size(); i++)
	{
		const BlinnPhongMaterial& material = static_cast<const BlinnPhongMaterial&>(*materials[i]);
		setMaterialUniforms(material);

		const std::vector<uint32_t>& meshesForTheCurrentMaterial = materialToMeshMapping.at(i);

		for (uint32_t meshIndex : meshesForTheCurrentMaterial)
		{
			const Model::Mesh& mesh = meshes[meshIndex];

			glm::mat4 meshTransform = transform * mesh.transform;
			m_blinnPhongShader->setUniformToValue("u_transform", meshTransform);

			const void* startOfIndices = reinterpret_cast<const void*>(static_cast<uint64_t>(sizeof(uint32_t)) * static_cast<uint64_t>(mesh.baseIndex));
			RendererUtilities::drawIndexedFromVertexOffset(mesh.indexCount, startOfIndices, mesh.baseVertex);
		}
	}
}

void BlinnPhongRendererImplementation::initialiseMultisampleFramebuffer()
{
	Framebuffer::FramebufferSpecification multisampleFramebufferSpecification;
	multisampleFramebufferSpecification.samples = 8;
	multisampleFramebufferSpecification.clearColor = { 0.1f, 0.1f, 0.1f, 1.0f };

	m_multisampleFramebuffer = createUnique<Framebuffer>(multisampleFramebufferSpecification);
}

void BlinnPhongRendererImplementation::initialiseDefaultMaterialTextures()
{
	uint8_t whitePixelImageData[4] = { 0xff, 0xff, 0xff, 0xff };
	m_defaultDiffuseMapTexture = createUnique<Texture>(static_cast<void*>(whitePixelImageData), 1, 1, 4);
	m_defaultSpecularMapTexture = createUnique<Texture>(static_cast<void*>(whitePixelImageData), 1, 1, 4);
}

void BlinnPhongRendererImplementation::setLightUniforms(const std::vector<Reference<PointLight>>& pointLights)
{
	m_blinnPhongShader->setUniformToValue("u_pointLightNumber", static_cast<uint32_t>(pointLights.size()));

	for (uint32_t i = 0; i < static_cast<uint32_t>(pointLights.size()); i++)
	{
		const BlinnPhongPointLight& pointLight = static_cast<const BlinnPhongPointLight&>(*pointLights[i]);
		std::string uniformName = "u_pointLights[" + std::to_string(i) + "]";
		m_blinnPhongShader->setUniformToValue(uniformName + ".worldPosition", pointLight.worldPosition);
		m_blinnPhongShader->setUniformToValue(uniformName + ".diffuseComponent", pointLight.diffuseComponent);
		m_blinnPhongShader->setUniformToValue(uniformName + ".specularComponent", pointLight.specularComponent);
		m_blinnPhongShader->setUniformToValue(uniformName + ".lightRadius", pointLight.lightRadius);
	}
}

void BlinnPhongRendererImplementation::setMaterialUniforms(const BlinnPhongMaterial& material)
{
	m_blinnPhongShader->setUniformToValue("u_material.diffuseColor", material.diffuseColor);
	m_blinnPhongShader->setUniformToValue("u_material.specularColor", material.specularColor);

	if (material.diffuseMap)
		material.diffuseMap->bind(0);
	else
		m_defaultDiffuseMapTexture->bind(0);

	if (material.specularMap)
		material.specularMap->bind(1);
	else
		m_defaultSpecularMapTexture->bind(1);

	if (material.normalMap)
	{
		material.normalMap->bind(2);
		m_blinnPhongShader->setUniformToValue("u_material.normalMap", 2);
		m_blinnPhongShader->setUniformToValue("u_material.useNormalMap", true);
	}
	else
		m_blinnPhongShader->setUniformToValue("u_material.useNormalMap", false);

	m_blinnPhongShader->setUniformToValue("u_material.diffuseMap", 0);
	m_blinnPhongShader->setUniformToValue("u_material.specularMap", 1);
	m_blinnPhongShader->setUniformToValue("u_material.shininess", material.shininess);
}
