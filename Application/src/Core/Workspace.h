#pragma once
#include "PCH.h"

#include "Renderer/WorkspaceCamera.h"
#include "Renderer/Texture.h"
#include "Scene/Scene.h"

class Workspace
{
public:

	Workspace();

	void onUpdate(TimeStep ts);

	void onWindowResizeEvent(uint32_t width, uint32_t height);
	void onMouseScrollEvent(float xOffset, float yOffset);

private:

	enum class SceneType
	{
		BLINN_PHONG = 0,
		PBR
	};

	void setSceneType(SceneType sceneType);

private:

	WorkspaceCamera m_camera;

	Reference<BlinnPhongScene> m_blinnPhongScene;
	Reference<PBRScene> m_PBRScene;
	Reference<Scene> m_currentScene;
};