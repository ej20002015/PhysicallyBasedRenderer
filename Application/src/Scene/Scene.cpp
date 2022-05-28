#include "PCH.h"
#include "Scene.h"

void Scene::addModel(const Reference<Model>& model, const glm::mat4& transform)
{
	m_modelsAndTransforms.push_back({ model, transform });
}

void Scene::addPointLight(const Reference<PointLight>& pointLight)
{
	m_pointLights.push_back(pointLight);
}
