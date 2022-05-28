#include "PCH.h"
#include "ModelFactory.h"

#include "glm/gtc/constants.hpp"

uint32_t ModelFactory::s_cubesCreatedCount = 0, ModelFactory::s_spheresCreatedCount = 0;

Reference<Model> ModelFactory::create(MeshType meshType, const Reference<Material>& material, uint32_t LOD)
{
	switch (meshType)
	{
	case ModelFactory::MeshType::CUBE:
		return createCube(material);
		break;
	case ModelFactory::MeshType::SPHERE:
		return createSphere(material, LOD);
		break;
	default:
		ASSERT_MESSAGE(false, "Unkown MeshType - cannot create a model of that type");
		return nullptr;
		break;
	}
}

Reference<Model> ModelFactory::createCube(const Reference<Material>& material)
{
	static std::vector<Model::Vertex> cubeVertices =
	{
		// Right face

		{ {  0.5f, -0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, { 0.0f, 0.0f }, {  0.0f,  0.0f, -1.0f }, {  0.0f,  1.0f,  0.0f } },
		{ {  0.5f, -0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f }, {  0.0f,  0.0f, -1.0f }, {  0.0f,  1.0f,  0.0f } },
		{ {  0.5f,  0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f }, {  0.0f,  0.0f, -1.0f }, {  0.0f,  1.0f,  0.0f } },
		{ {  0.5f,  0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, { 0.0f, 1.0f }, {  0.0f,  0.0f, -1.0f }, {  0.0f,  1.0f,  0.0f } },

		// Left face

		{ { -0.5f, -0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, { 0.0f, 0.0f }, {  0.0f,  0.0f,  1.0f }, {  0.0f,  1.0f,  0.0f } },
		{ { -0.5f, -0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f }, {  0.0f,  0.0f,  1.0f }, {  0.0f,  1.0f,  0.0f } },
		{ { -0.5f,  0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f }, {  0.0f,  0.0f,  1.0f }, {  0.0f,  1.0f,  0.0f } },
		{ { -0.5f,  0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, { 0.0f, 1.0f }, {  0.0f,  0.0f,  1.0f }, {  0.0f,  1.0f,  0.0f } },

		// Top face

		{ { -0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, { 0.0f, 0.0f }, {  1.0f,  0.0f,  0.0f }, {  0.0f,  0.0f, -1.0f } },
		{ {  0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, { 1.0f, 0.0f }, {  1.0f,  0.0f,  0.0f }, {  0.0f,  0.0f, -1.0f } },
		{ {  0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, { 1.0f, 1.0f }, {  1.0f,  0.0f,  0.0f }, {  0.0f,  0.0f, -1.0f } },
		{ { -0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, { 0.0f, 1.0f }, {  1.0f,  0.0f,  0.0f }, {  0.0f,  0.0f, -1.0f } },

		// Bottom face

		{ { -0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, { 0.0f, 0.0f }, {  1.0f,  0.0f,  0.0f }, {  0.0f,  0.0f,  1.0f } },
		{ {  0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, { 1.0f, 0.0f }, {  1.0f,  0.0f,  0.0f }, {  0.0f,  0.0f,  1.0f } },
		{ {  0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, { 1.0f, 1.0f }, {  1.0f,  0.0f,  0.0f }, {  0.0f,  0.0f,  1.0f } },
		{ { -0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, { 0.0f, 1.0f }, {  1.0f,  0.0f,  0.0f }, {  0.0f,  0.0f,  1.0f } },

		// Front face

		{ { -0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 0.0f, 0.0f }, {  1.0f,  0.0f,  0.0f }, {  0.0f,  1.0f,  0.0f } },
		{ {  0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 0.0f }, {  1.0f,  0.0f,  0.0f }, {  0.0f,  1.0f,  0.0f } },
		{ {  0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 1.0f }, {  1.0f,  0.0f,  0.0f }, {  0.0f,  1.0f,  0.0f } },
		{ { -0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 0.0f, 1.0f }, {  1.0f,  0.0f,  0.0f }, {  0.0f,  1.0f,  0.0f } },

		// Back face

		{ {  0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 0.0f, 0.0f }, { -1.0f,  0.0f,  0.0f }, {  0.0f,  1.0f,  0.0f } },
		{ { -0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 0.0f }, { -1.0f,  0.0f,  0.0f }, {  0.0f,  1.0f,  0.0f } },
		{ { -0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 1.0f }, { -1.0f,  0.0f,  0.0f }, {  0.0f,  1.0f,  0.0f } },
		{ {  0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 0.0f, 1.0f }, { -1.0f,  0.0f,  0.0f }, {  0.0f,  1.0f,  0.0f } }
	};

	static std::vector<Model::TriangleIndex> cubeTriangleIndices =
	{
		{  0,  1,  2 },
		{  0,  2,  3 },

		{  4,  5,  6 },
		{  4,  6,  7 },

		{  8,  9, 10 },
		{  8, 10, 11 },

		{ 12, 13, 14 },
		{ 12, 14, 15 },

		{ 16, 17, 18 },
		{ 16, 18, 19 },

		{ 20, 21, 22 },
		{ 20, 22, 23 }
	};

	std::string modelIdentifier = "CUBE_" + std::to_string(s_cubesCreatedCount);
	s_cubesCreatedCount = (s_cubesCreatedCount + 1) % std::numeric_limits<uint32_t>::max();

	return createReference<Model>(modelIdentifier, cubeVertices, cubeTriangleIndices, material);
}

Reference<Model> ModelFactory::createSphere(const Reference<Material>& material, uint32_t LOD)
{
	auto vertices = generateSphereVertices(LOD);
	auto triangleIndices = generateSphereTriangleIndices(LOD, vertices);
	setTangentsAndBitangents(vertices, triangleIndices);

	std::string modelIdentifier = "SPHERE_" + std::to_string(s_spheresCreatedCount);
	s_spheresCreatedCount = (s_spheresCreatedCount + 1) % std::numeric_limits<uint32_t>::max();

	return createReference<Model>(modelIdentifier, vertices, triangleIndices, material);
}

void ModelFactory::setTangentsAndBitangents(std::vector<Model::Vertex>& vertices, const std::vector<Model::TriangleIndex>& triangleIndices)
{
	for (const Model::TriangleIndex& triangleIndex : triangleIndices)
	{
		// Solve using a linear system of equations to get tangent and bitangent

		Model::Vertex& vertex1 = vertices[triangleIndex.vertex1];
		Model::Vertex& vertex2 = vertices[triangleIndex.vertex2];
		Model::Vertex& vertex3 = vertices[triangleIndex.vertex3];

		glm::vec3 edge1 = vertex2.position - vertex1.position;
		glm::vec3 edge2 = vertex3.position - vertex2.position;

		float deltaU1 = vertex2.textureCoordinates.x - vertex1.textureCoordinates.x;
		float deltaU2 = vertex3.textureCoordinates.x - vertex2.textureCoordinates.x;
		float deltaV1 = vertex2.textureCoordinates.y - vertex1.textureCoordinates.y;
		float deltaV2 = vertex3.textureCoordinates.y - vertex2.textureCoordinates.y;

		glm::mat2x3 B =
		{
			edge1.x, edge1.y, edge1.z,
			edge2.x, edge2.y, edge2.z
		};

		glm::mat2x2 A =
		{
			deltaU1, deltaU2,
			deltaV1, deltaV2
		};

		glm::mat2x3 X = B * glm::transpose(glm::inverse(A));

		glm::vec3 tangent = glm::normalize(X[0]);
		glm::vec3 bitangent = glm::normalize(X[1]);

		// Assign to all the vertices in the current triangle

		vertex1.tangent = tangent;
		vertex1.bitangent = bitangent;
		vertex2.tangent = tangent;
		vertex2.bitangent = bitangent;
		vertex3.tangent = tangent;
		vertex3.bitangent = bitangent;
	}
}

std::vector<Model::Vertex> ModelFactory::generateSphereVertices(uint32_t LOD)
{
	uint32_t size = LOD;
	std::vector<Model::Vertex> vertices;

	/*
	* Generation of vertices adapted from http://www.songho.ca/opengl/gl_sphere.html
	*/

	float sectorStepSize = glm::two_pi<float>() / static_cast<float>(size);
	float stackStepSize = glm::pi<float>() / static_cast<float>(size);

	for (uint32_t i = 0; i < size + 1; i++)
	{
		float stackAngle = glm::half_pi<float>() - stackStepSize * static_cast<float>(i);
		float halfCosStackAngle = 0.5f * glm::cos(stackAngle);
		float halfSinStackAngle = 0.5f * glm::sin(stackAngle);

		for (uint32_t j = 0; j < size + 1; j++)
		{
			float sectorAngle = sectorStepSize * static_cast<float>(j);

			float x = halfCosStackAngle * glm::sin(sectorAngle);
			float y = halfSinStackAngle;
			float z = halfCosStackAngle * glm::cos(sectorAngle);

			glm::vec3 vertexPosition = { x, y, z };
			glm::vec3 vertexNormal = glm::normalize(glm::vec3(x, y, z));

			glm::vec2 vertexTextureCoordinates =
			{
				static_cast<float>(j) / size,
				1.0f - static_cast<float>(i) / size
			};

			Model::Vertex vertex;
			vertex.position = vertexPosition;
			vertex.normal = vertexNormal;
			vertex.textureCoordinates = vertexTextureCoordinates;
			vertices.push_back(vertex);
		}
	}

	return vertices;
}

std::vector<Model::TriangleIndex> ModelFactory::generateSphereTriangleIndices(uint32_t LOD, const std::vector<Model::Vertex>& vertices)
{
	uint32_t size = LOD;
	std::vector<Model::TriangleIndex> triangleIndices;

	/*
	* Generation of indices adapted from http://www.songho.ca/opengl/gl_sphere.html
	*/

	// Iterate through each layer of horizontal vertices, from top to bottom
	for (uint32_t i = 0; i < size; i++)
	{
		uint32_t indexOfTopLeftVertex = i * (size + 1);
		uint32_t indexOfBottomLeftVertex = indexOfTopLeftVertex + size + 1;

		// Iterate 360 degrees around the sphere, joining the vertices in the current row into triangles
		for (uint32_t j = 0; j < size; j++, indexOfTopLeftVertex++, indexOfBottomLeftVertex++)
		{
			if (i)
			{
				Model::TriangleIndex triangle1Index =
				{
					indexOfTopLeftVertex,
					indexOfBottomLeftVertex,
					indexOfTopLeftVertex + 1
				};
				triangleIndices.push_back(triangle1Index);
			}

			if (i != (size - 1))
			{
				Model::TriangleIndex triangle2Index
				{
					indexOfTopLeftVertex + 1,
					indexOfBottomLeftVertex,
					indexOfBottomLeftVertex + 1
				};
				triangleIndices.push_back(triangle2Index);
			}
		}
	}

	return triangleIndices;
}
