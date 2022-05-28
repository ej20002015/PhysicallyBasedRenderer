#include "PCH.h"
#include "HDRTestScene.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/constants.hpp"

#include "Scene/Model.h"
#include "Scene/ModelFactory.h"

HDRTestScene::HDRTestScene()
{
	glm::vec3 pointLightPosition = { 0.00f, 0.62f, 3.45f };
	glm::vec3 lightFixturePosition = { 0.00f, 0.75f, 3.05f };
	glm::mat4 wallTransform = glm::translate(glm::mat4(1.0f), { 0.0f, 0.0f, 3.0f }) * glm::scale(glm::mat4(1.0f), { 2.0f, 2.0f, 0.1f });
	float bulbLumens = 50.0f;
	float blinnPhongLightValue = 1.0f;
	uint32_t lightNumber = 10;

	Texture::TextureSpecification wallDiffuseBaseColorTextureSpecification;
	wallDiffuseBaseColorTextureSpecification.SRGB = true;
	wallDiffuseBaseColorTextureSpecification.filePath = "Assets/Textures/ParticleBoard/ParticleBoardBaseColor.jpg";
	Reference<Texture> wallDiffuseBaseColorTexture = createReference<Texture>(wallDiffuseBaseColorTextureSpecification);

	// BLINN-PHONG SCENE

	{
		m_blinnPhongScene = BlinnPhongScene::create();

		// Lights

		for (uint32_t i = 0; i < lightNumber; i++)
		{
			auto pointLight = BlinnPhongPointLight::create();
			pointLight->worldPosition = pointLightPosition;
			pointLight->diffuseComponent = glm::vec3(1.0f, 1.0f, 1.0f) * blinnPhongLightValue;
			pointLight->specularComponent = glm::vec3(1.0f, 1.0f, 1.0f) * blinnPhongLightValue;
			m_blinnPhongScene->addPointLight(pointLight);
		}

		// Models

		auto wallMaterial = BlinnPhongMaterial::create();
		wallMaterial->diffuseMap = wallDiffuseBaseColorTexture;
		wallMaterial->specularColor = glm::vec3{ 1.0f, 1.0f, 1.0f } * 0.2f;
		wallMaterial->shininess = 40.0f;

		Reference<Model> wallModel = ModelFactory::create(ModelFactory::MeshType::CUBE, wallMaterial);
		m_blinnPhongScene->addModel(wallModel, wallTransform);

		try
		{
			auto lightFixtureModel = Model::create("Assets/Models/LightFixture/LightFixture.obj", Model::MaterialModel::BLINN_PHONG);
			glm::mat4 rotationTransform = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), { 0.0f, 1.0f, 0.0f });
			m_blinnPhongScene->addModel(lightFixtureModel, glm::translate(glm::mat4(1.0f), lightFixturePosition) * rotationTransform);
		}
		catch (Model::ModelCreationException& e)
		{
			Log::error(e.what());
		}
	}

	// PBR SCENE

	{
		m_PBRScene = PBRScene::create();

		m_PBRScene->setExposureLevel(1.0f);

		// Lights
		for (uint32_t i = 0; i < lightNumber; i++)
		{
			auto pointLight = PBRPointLight::create();
			pointLight->worldPosition = pointLightPosition;
			pointLight->luminousPower = bulbLumens;
			m_PBRScene->addPointLight(pointLight);
		}

		// Models

		auto wallMaterial = PBRMaterial::create();
		wallMaterial->baseColorMap = wallDiffuseBaseColorTexture;
		wallMaterial->roughness = 0.455f;
		wallMaterial->metalness = 0.0f;
		//wallMaterial->metalnessMap = wallMetalnessTexture;

		Reference<Model> wallModel = ModelFactory::create(ModelFactory::MeshType::CUBE, wallMaterial);
		m_PBRScene->addModel(wallModel, wallTransform);

		try
		{
			auto lightFixtureModel = Model::create("Assets/Models/LightFixture/LightFixture.fbx", Model::MaterialModel::PBR);
			glm::mat4 rotationTransform = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), { 0.0f, 1.0f, 0.0f });
			m_PBRScene->addModel(lightFixtureModel, glm::translate(glm::mat4(1.0f), lightFixturePosition) * rotationTransform);
		}
		catch (Model::ModelCreationException& e)
		{
			Log::error(e.what());
		}
	}
}
