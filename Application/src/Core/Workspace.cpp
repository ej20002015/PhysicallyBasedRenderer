#include "PCH.h"
#include "Workspace.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Renderer/Renderer.h"
#include "Renderer/Material.h"

#include "Scene/ModelFactory.h"

#include "TestScenes/TestSceneFactory.h"

#include "Application.h"

Workspace::Workspace()
{
	// BLINN-PHONG SCENE

	{
		m_blinnPhongScene = BlinnPhongScene::create();

		// Lights

		auto pointLight = BlinnPhongPointLight::create();
		pointLight->worldPosition = { 1.0f, 1.0f, 0.0f };
		pointLight->diffuseComponent = { 1.0f, 1.0f, 1.0f };
		pointLight->specularComponent = { 1.0f, 1.0f, 1.0f };
		m_blinnPhongScene->addPointLight(pointLight);

		// Models

		try
		{
			auto lightFixtureModel = Model::create("Assets/Models/Backpack/Backpack.obj", Model::MaterialModel::BLINN_PHONG);
			m_blinnPhongScene->addModel(lightFixtureModel, glm::mat4(1.0f));
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

		auto pointLight = PBRPointLight::create();
		pointLight->worldPosition = { 1.0f, 1.0f, 0.0f };
		m_PBRScene->addPointLight(pointLight);

		try
		{
			auto lightFixtureModel = Model::create("Assets/Models/Pistol/Pistol.fbx", Model::MaterialModel::PBR);
			m_PBRScene->addModel(lightFixtureModel, glm::mat4(1.0f));
		}
		catch (Model::ModelCreationException& e)
		{
			Log::error(e.what());
		}
	}

	m_currentScene = m_blinnPhongScene;
	Renderer::setRendererType(Renderer::RendererType::BLINN_PHONG);

	Log::info("Initialised the workspace");
}

void Workspace::onUpdate(TimeStep ts)
{
	m_camera.onUpdate(ts);
	
	Renderer::drawScene(m_currentScene, m_camera);

	if (Application::getInput().isKeyPressed(KeyCode::KEY_P))
		setSceneType(SceneType::PBR);
	else if (Application::getInput().isKeyPressed(KeyCode::KEY_B))
		setSceneType(SceneType::BLINN_PHONG);
}

void Workspace::onWindowResizeEvent(uint32_t width, uint32_t height)
{
	m_camera.onWindowResizeEvent(width, height);
}

void Workspace::onMouseScrollEvent(float xOffset, float yOffset)
{
	m_camera.onMouseScrollEvent(xOffset, yOffset);
}

void Workspace::setSceneType(SceneType sceneType)
{
	switch (sceneType)
	{
	case Workspace::SceneType::BLINN_PHONG:
		m_currentScene = m_blinnPhongScene;
		Renderer::setRendererType(Renderer::RendererType::BLINN_PHONG);
		Log::info("Switched to Blinn-Phong scene");
		break;
	case Workspace::SceneType::PBR:
		m_currentScene = m_PBRScene;
		Renderer::setRendererType(Renderer::RendererType::PBR);
		Log::info("Switched to PBR scene");
		break;
	default:
		ASSERT_MESSAGE(false, "Unknown sceneType");
		break;
	}
}
