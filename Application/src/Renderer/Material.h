#pragma once
#include "PCH.h"

#include "glm/glm.hpp"

#include "Texture.h"

struct Material
{
	Reference<const Texture> normalMap;
};

struct BlinnPhongMaterial : public Material
{
	static Reference<BlinnPhongMaterial> create() { return createReference<BlinnPhongMaterial>(); }

	glm::vec4 diffuseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	glm::vec3 specularColor = { 1.0f, 1.0f, 1.0f };
	Reference<const Texture> diffuseMap;
	Reference<const Texture> specularMap;
	float shininess = 32.0f;
};

struct PBRMaterial : public Material
{
	static Reference<PBRMaterial> create() { return createReference<PBRMaterial>(); }

	glm::vec4 baseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	float roughness = 1.0f;
	float metalness = 1.0f;
	Reference<const Texture> baseColorMap;
	Reference<const Texture> roughnessMap;
	Reference<const Texture> metalnessMap;
};