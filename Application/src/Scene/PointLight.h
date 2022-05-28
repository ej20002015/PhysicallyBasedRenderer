#pragma once
#include "PCH.h"

#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

struct PointLight
{
	virtual ~PointLight() = default;

	glm::vec3 worldPosition = { 0.0f, 0.0f, 0.0f };
	float lightRadius = 10.0f;
};

struct BlinnPhongPointLight : public PointLight
{
	static Reference<BlinnPhongPointLight> create() { return createReference<BlinnPhongPointLight>(); }

	glm::vec3 diffuseComponent = { 1.0f, 1.0f, 1.0f };
	glm::vec3 specularComponent = { 1.0f, 1.0f, 1.0f };
};

struct PBRPointLight : public PointLight
{
	static Reference<PBRPointLight> create() { return createReference<PBRPointLight>(); }

	glm::vec3 lightColor = { 1.0f, 1.0f, 1.0f };
	// PI becomes the default luminous intensity in the shader
	float luminousPower = 4.0f * glm::pi<float>() * glm::pi<float>();
};