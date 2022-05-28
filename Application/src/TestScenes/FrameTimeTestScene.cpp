#include "PCH.h"
#include "FrameTimeTestScene.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Scene/ModelFactory.h"

FrameTimeTestScene::FrameTimeTestScene()
{
	uint32_t modelCount = 3000;
	uint32_t lightCount = 100;

	// BLINN-PHONG SCENE

	{
		m_blinnPhongScene = BlinnPhongScene::create();

		// Lights

		float xPosition = 0.0f;
		float zPosition = 0.0f;

		for (uint32_t i = 0; i < lightCount; i++)
		{
			auto pointLight = BlinnPhongPointLight::create();
			pointLight->worldPosition = glm::vec3(xPosition, 0.0f, zPosition);
			pointLight->diffuseComponent = { 1.0f, 1.0f, 1.0f };
			pointLight->specularComponent = { 1.0f, 1.0f, 1.0f };
			m_blinnPhongScene->addPointLight(pointLight);
			xPosition += 1.0f;
			if (i % 10 == 0)
			{
				xPosition = 0.0f;
				zPosition += 1.0f;
			}
		}

		// Models

		Reference<Model> lightFixtureModel;

		try
		{
			lightFixtureModel = Model::create("Assets/Models/LightFixture/LightFixture.obj", Model::MaterialModel::BLINN_PHONG);
		}
		catch (Model::ModelCreationException& e)
		{
			Log::error(e.what());
		}

		Log::info("Triangle Count: {0}", modelCount * lightFixtureModel->getTriangleCount());

		xPosition = 0.0f;
		zPosition = 0.0f;

		for (uint32_t i = 0; i < modelCount; i++)
		{
			m_blinnPhongScene->addModel(lightFixtureModel, glm::translate(glm::mat4(1.0f), { xPosition, 0.0f, zPosition }));
			xPosition += 1.0f;
			if (i % (modelCount / 10) == 0)
			{
				xPosition = 0.0f;
				zPosition += 1.0f;
			}
		}
	}

	// PBR SCENE

	{
		m_PBRScene = PBRScene::create();

		m_PBRScene->setExposureLevel(1.0f);

		// Lights

		float xPosition = 0.0f;
		float zPosition = 0.0f;

		for (uint32_t i = 0; i < lightCount; i++)
		{
			auto pointLight = PBRPointLight::create();
			pointLight->worldPosition = glm::vec3(xPosition, 0.0f, zPosition);
			pointLight->luminousPower = 50.0f;
			m_PBRScene->addPointLight(pointLight);
			xPosition += 1.0f;
			if (i % 10 == 0)
			{
				xPosition = 0.0f;
				zPosition += 1.0f;
			}
		}

		// Models

		Reference<Model> lightFixtureModel;

		try
		{
			lightFixtureModel = Model::create("Assets/Models/LightFixture/LightFixture.fbx", Model::MaterialModel::PBR);
		}
		catch (Model::ModelCreationException& e)
		{
			Log::error(e.what());
		}

		xPosition = 0.0f;
		zPosition = 0.0f;

		for (uint32_t i = 0; i < modelCount; i++)
		{
			m_PBRScene->addModel(lightFixtureModel, glm::translate(glm::mat4(1.0f), { xPosition, 0.0f, zPosition }));
			xPosition += 1.0f;
			if (i % (modelCount / 10) == 0)
			{
				xPosition = 0.0f;
				zPosition += 1.0f;
			}
		}
	}
}

