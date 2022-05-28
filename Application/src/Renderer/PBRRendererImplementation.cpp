#include "PCH.h"
#include "PBRRendererImplementation.h"

PBRRendererImplementation::PBRRendererImplementation()
{
	initialiseHDRMultisampleFramebuffer();
	initialiseIntermediateHDRFramebuffer();

	m_PBRShader = createUnique<Shader>(std::initializer_list<std::string>
	{
		"Assets/Shaders/PBR.glsl.vert",
		"Assets/Shaders/PBR.glsl.frag"
	});

	m_postProcessingShader = createUnique<Shader>(std::initializer_list<std::string>
	{
		"Assets/Shaders/PBRPostProcessing.glsl.vert",
		"Assets/Shaders/PBRPostProcessing.glsl.frag"
	});

	initialiseDefaultMaterialTextures();
	initialiseQuadBuffers();

	Log::info("PBR renderer initialised");
}

void PBRRendererImplementation::onWindowResizeEvent(uint32_t width, uint32_t height)
{
	m_multisampleHDRFramebuffer->onWindowResizeEvent(width, height);
	m_intermediateHDRFramebuffer->onWindowResizeEvent(width, height);
}

void PBRRendererImplementation::beginScene(const Camera& camera, const std::vector<Reference<PointLight>>& pointLights)
{
	Log::trace("Beginning to render a PBR scene");

	m_multisampleHDRFramebuffer->bind();
	m_multisampleHDRFramebuffer->clear();

	m_PBRShader->bind();

	m_PBRShader->setUniformToValue("u_projectionViewMatrix", camera.getProjectionMatrix() * camera.getViewMatrix());
	m_PBRShader->setUniformToValue("u_viewPosition", camera.getCameraPosition());

	setLightUniforms(pointLights);
}

void PBRRendererImplementation::endScene(float exposureLevel)
{
	m_multisampleHDRFramebuffer->blitToTargetFramebuffer(m_intermediateHDRFramebuffer);

	m_quadVertexBuffer->bind();
	m_quadIndexBuffer->bind();

	RendererUtilities::bindDefaultFramebuffer();
	
	m_postProcessingShader->bind();

	m_intermediateHDRFramebuffer->bindColorAttachment();
	m_postProcessingShader->setUniformToValue("u_inputTexture", 0);

	m_postProcessingShader->setUniformToValue("u_exposure", exposureLevel);

	RendererUtilities::drawIndexed(m_quadIndexBuffer->getCount());

	Log::trace("Ended the rendering of a PBR scene");
}

void PBRRendererImplementation::drawScene(Reference<Scene> scene, const Camera& camera)
{
	Log::trace("Drawing PBR scene");

	beginScene(camera, scene->getPointLights());

	for (auto modelAndTransform : scene->getModelsAndTransforms())
		drawModel(modelAndTransform.first, modelAndTransform.second);

	float exposure = std::static_pointer_cast<PBRScene>(scene)->getExposureLevel();
	endScene(exposure);
}

void PBRRendererImplementation::drawModel(Reference<Model> model, const glm::mat4& transform)
{
	Log::trace("Drawing PBR model {0} with {1} meshes", model->getModelIdentifier(), static_cast<uint32_t>(model->getMeshes().size()));

	model->getVertexBuffer()->bind();
	model->getIndexBuffer()->bind();

	const std::vector<Reference<Material>>& materials = model->getMaterials();
	const auto& materialToMeshMapping = model->getMaterialToMeshMapping();
	const std::vector<Model::Mesh>& meshes = model->getMeshes();

	for (uint32_t i = 0; i < materials.size(); i++)
	{
		const PBRMaterial& material = static_cast<const PBRMaterial&>(*materials[i]);
		setMaterialUniforms(material);

		const std::vector<uint32_t>& meshesForTheCurrentMaterial = materialToMeshMapping.at(i);

		for (uint32_t meshIndex : meshesForTheCurrentMaterial)
		{
			const Model::Mesh& mesh = meshes[meshIndex];

			glm::mat4 meshTransform = transform * mesh.transform;
			m_PBRShader->setUniformToValue("u_transform", meshTransform);

			const void* startOfIndices = reinterpret_cast<const void*>(static_cast<uint64_t>(sizeof(uint32_t)) * static_cast<uint64_t>(mesh.baseIndex));
			RendererUtilities::drawIndexedFromVertexOffset(mesh.indexCount, startOfIndices, mesh.baseVertex);
		}
	}
}

void PBRRendererImplementation::initialiseHDRMultisampleFramebuffer()
{
	Framebuffer::FramebufferSpecification multisampleHDRFramebufferSpecification;
	multisampleHDRFramebufferSpecification.colorAttachmentFormat = Framebuffer::ColorAttachmentFormat::RGBA16F;
	multisampleHDRFramebufferSpecification.samples = 8;
	multisampleHDRFramebufferSpecification.clearColor = { 0.1f, 0.1f, 0.1f, 1.0f };

	m_multisampleHDRFramebuffer = createUnique<Framebuffer>(multisampleHDRFramebufferSpecification);
}

void PBRRendererImplementation::initialiseIntermediateHDRFramebuffer()
{
	Framebuffer::FramebufferSpecification intermediateHDRFramebufferSpecification;
	intermediateHDRFramebufferSpecification.colorAttachmentFormat = Framebuffer::ColorAttachmentFormat::RGBA16F;

	m_intermediateHDRFramebuffer = createReference<Framebuffer>(intermediateHDRFramebufferSpecification);
}

void PBRRendererImplementation::initialiseDefaultMaterialTextures()
{
	uint8_t whitePixelImageData[4] = { 0xff, 0xff, 0xff, 0xff };
	uint8_t monoChannelSaturatedPixelImageData = 0xff;

	m_defaultBaseColorMapTexture = createUnique<Texture>(static_cast<void*>(whitePixelImageData), 1, 1, 4);
	m_defaultRoughnessMapTexture = createUnique<Texture>(static_cast<void*>(&monoChannelSaturatedPixelImageData), 1, 1, 1);
	m_defaultMetalnessMapTexture = createUnique<Texture>(static_cast<void*>(&monoChannelSaturatedPixelImageData), 1, 1, 1);
}

void PBRRendererImplementation::initialiseQuadBuffers()
{
	static constexpr float quadVertices[] =
	{
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f,
		-1.0f,  1.0f,  0.0f, 1.0f
	};

	static constexpr uint32_t quadIndices[6] =
	{
		0, 1, 2,
		0, 2, 3
	};

	VertexBufferLayout vertexBufferLayout =
	{
		{ ShaderDataType::FLOAT2, "a_position" },
		{ ShaderDataType::FLOAT2, "a_textureCoordinates"},
	};

	m_quadVertexBuffer = createUnique<VertexBuffer>(static_cast<const void*>(quadVertices), sizeof(quadVertices), vertexBufferLayout);
	m_quadIndexBuffer = createUnique<IndexBuffer>(quadIndices, 6);
}

void PBRRendererImplementation::setLightUniforms(const std::vector<Reference<PointLight>>& pointLights)
{
	m_PBRShader->setUniformToValue("u_pointLightNumber", static_cast<uint32_t>(pointLights.size()));

	for (uint32_t i = 0; i < static_cast<uint32_t>(pointLights.size()); i++)
	{
		const PBRPointLight& pointLight = static_cast<const PBRPointLight&>(*pointLights[i]);
		std::string uniformName = "u_pointLights[" + std::to_string(i) + "]";
		m_PBRShader->setUniformToValue(uniformName + ".worldPosition", pointLight.worldPosition);
		m_PBRShader->setUniformToValue(uniformName + ".lightColor", pointLight.lightColor);
		m_PBRShader->setUniformToValue(uniformName + ".luminousPower", pointLight.luminousPower);
		m_PBRShader->setUniformToValue(uniformName + ".lightRadius", pointLight.lightRadius);
	}
}

void PBRRendererImplementation::setMaterialUniforms(const PBRMaterial& material)
{
	m_PBRShader->setUniformToValue("u_material.baseColor", material.baseColor);
	m_PBRShader->setUniformToValue("u_material.roughness", material.roughness);
	m_PBRShader->setUniformToValue("u_material.metalness", material.metalness);

	if (material.baseColorMap)
		material.baseColorMap->bind(0);
	else
		m_defaultBaseColorMapTexture->bind(0);

	if (material.roughnessMap)
		material.roughnessMap->bind(1);
	else
		m_defaultRoughnessMapTexture->bind(1);

	if (material.metalnessMap)
		material.metalnessMap->bind(2);
	else
		m_defaultMetalnessMapTexture->bind(2);

	if (material.normalMap)
	{
		material.normalMap->bind(3);
		m_PBRShader->setUniformToValue("u_material.normalMap", 3);
		m_PBRShader->setUniformToValue("u_material.useNormalMap", true);
	}
	else
		m_PBRShader->setUniformToValue("u_material.useNormalMap", false);

	m_PBRShader->setUniformToValue("u_material.baseColorMap", 0);
	m_PBRShader->setUniformToValue("u_material.roughnessMap", 1);
	m_PBRShader->setUniformToValue("u_material.metalnessMap", 2);
}
