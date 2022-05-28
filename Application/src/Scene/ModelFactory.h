#pragma once
#include "PCH.h"

#include "Model.h"

class ModelFactory
{
public:

	enum class MeshType
	{
		CUBE = 0,
		SPHERE
	};

	static constexpr uint32_t DEFAULT_LOD = 32;

public:

	static Reference<Model> create(MeshType meshType, const Reference<Material>& material, uint32_t LOD = DEFAULT_LOD);

private:

	static Reference<Model> createCube(const Reference<Material>& material);
	static Reference<Model> createSphere(const Reference<Material>& material, uint32_t LOD);

	static void setTangentsAndBitangents(std::vector<Model::Vertex>& vertices, const std::vector<Model::TriangleIndex>& triangleIndices);

	static std::vector<Model::Vertex> generateSphereVertices(uint32_t LOD);
	static std::vector<Model::TriangleIndex> generateSphereTriangleIndices(uint32_t LOD, const std::vector<Model::Vertex>& vertices);

private:

	static uint32_t s_cubesCreatedCount;
	static uint32_t s_spheresCreatedCount;
};
