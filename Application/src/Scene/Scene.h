#pragma once
#include "PCH.h"

#include "glm/glm.hpp"

#include "PointLight.h"
#include "Model.h"

class Scene
{
public:

	Scene() = default;
	virtual ~Scene() = default;

	void addModel(const Reference<Model>& model, const glm::mat4& transform);
	void addPointLight(const Reference<PointLight>& pointLight);

	const std::vector<Reference<PointLight>>& getPointLights() const { return m_pointLights; }
	const std::vector<std::pair<Reference<Model>, glm::mat4>>& getModelsAndTransforms() const { return m_modelsAndTransforms; }

private:

	std::vector<std::pair<Reference<Model>, glm::mat4>> m_modelsAndTransforms;
	std::vector<Reference<PointLight>> m_pointLights;
};

class BlinnPhongScene : public Scene
{
public:

	BlinnPhongScene() = default;

	static Reference<BlinnPhongScene> create() { return createReference<BlinnPhongScene>(); }
};

class PBRScene : public Scene
{
public:

	PBRScene() = default;

	static Reference<PBRScene> create() { return createReference<PBRScene>(); }

	float getExposureLevel() const { return m_exposureLevel; }
	void setExposureLevel(float exposureLevel) { m_exposureLevel = exposureLevel; }

private:

	float m_exposureLevel = 1.0f;
};