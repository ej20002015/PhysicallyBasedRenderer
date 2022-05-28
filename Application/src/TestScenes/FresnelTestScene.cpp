#include "PCH.h"
#include "FresnelTestScene.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Scene/ModelFactory.h"
#include "Renderer/Texture.h"

FresnelTestScene::FresnelTestScene()
{
	glm::vec3 pointLightPosition = { 0.0f, 0.5f, 0.0f };
	glm::mat4 woodenFloorTransform = glm::scale(glm::mat4(1.0f), { 4.0f, 0.1f, 4.0f });

	Texture::TextureSpecification woodenFloorBaseColorSpecification;
	woodenFloorBaseColorSpecification.filePath = "Assets/Textures/WoodenFloor/WoodenFloorBaseColor.jpg";
	woodenFloorBaseColorSpecification.SRGB = true;
	Reference<Texture> woodenFloorBaseColor = createReference<Texture>(woodenFloorBaseColorSpecification);

	// BLINN-PHONG SCENE

	{
		m_blinnPhongScene = BlinnPhongScene::create();

		// Lights

		auto pointLight = BlinnPhongPointLight::create();
		pointLight->worldPosition = pointLightPosition;
		pointLight->diffuseComponent = { 0.6f, 0.6f, 0.6f };
		pointLight->specularComponent = { 0.6f, 0.6f, 0.6f };
		m_blinnPhongScene->addPointLight(pointLight);

		// Models

		auto woodenFloorMaterial = BlinnPhongMaterial::create();
		woodenFloorMaterial->diffuseMap = woodenFloorBaseColor;
		woodenFloorMaterial->shininess = 32.0f;

		Reference<Model> woodenFloorModel = ModelFactory::create(ModelFactory::MeshType::CUBE, woodenFloorMaterial);
		m_blinnPhongScene->addModel(woodenFloorModel, woodenFloorTransform);
	}

	// PBR SCENE

	{
		m_PBRScene = PBRScene::create();

		m_PBRScene->setExposureLevel(1.0f);

		// Lights

		auto pointLight = PBRPointLight::create();
		pointLight->worldPosition = pointLightPosition;
		m_PBRScene->addPointLight(pointLight);

		// Models

		auto woodenFloorMaterial = PBRMaterial::create();
		woodenFloorMaterial->baseColorMap = woodenFloorBaseColor;
		woodenFloorMaterial->roughness = 0.5f;
		woodenFloorMaterial->metalness = 0.0f;

		Reference<Model> woodenFloorModel = ModelFactory::create(ModelFactory::MeshType::CUBE, woodenFloorMaterial);
		m_PBRScene->addModel(woodenFloorModel, woodenFloorTransform);
	}
}
