#pragma once
#include "PCH.h"

#include "glm/glm.hpp"
#include "assimp/scene.h"

#include "Renderer/VertexBuffer.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/Material.h"
#include "Renderer/Texture.h"

class ModelFactory;

class Model
{
public:

	struct ModelCreationException : public std::exception
	{
		std::string errorMessage;

		ModelCreationException(const std::string errorMessage)
			: errorMessage("ModelCreationException Occured: " + errorMessage) {}

		const char* what() const override
		{
			return errorMessage.c_str();
		}
	};

	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 textureCoordinates;
		glm::vec3 tangent;
		glm::vec3 bitangent;
	};

	struct TriangleIndex
	{
		uint32_t vertex1, vertex2, vertex3;
	};

	struct Mesh
	{
		glm::mat4 transform;
		uint32_t vertexCount;
		uint32_t indexCount;
		uint32_t baseVertex;
		uint32_t baseIndex;
		std::string name;
	};

	enum class MaterialModel
	{
		BLINN_PHONG = 0,
		PBR
	};

public:
	
	Model() = delete;
	Model(const std::string& filePath, MaterialModel materialModel);
	Model(const std::string modelIdentifier, const std::vector<Vertex>& vertices, const std::vector<TriangleIndex>& triangleIndices, const Reference<Material>& material);
	Model(const Model&) = delete;

	static Reference<Model> create(const std::string& filePath, MaterialModel materialModel) { return createReference<Model>(filePath, materialModel); }

	const std::string& getModelIdentifier() const { return m_modelIdentifier; }

	const std::vector<Mesh>& getMeshes() { return m_meshes; }

	const Reference<VertexBuffer>& getVertexBuffer() { return m_vertexBuffer; }
	const Reference<IndexBuffer>& getIndexBuffer() { return m_indexBuffer; }

	const std::vector<Reference<Material>>& getMaterials() { return m_materials; }
	const std::unordered_map<uint32_t, std::vector<uint32_t>>& getMaterialToMeshMapping() { return m_materialToMeshMapping; }

	uint32_t getVertexCount() const { return static_cast<uint32_t>(m_vertices.size()); }
	uint32_t getTriangleCount() const { return static_cast<uint32_t>(m_triangleIndices.size()); }

private:

	void processMeshes();
	void processModelGraph();
	void createBuffers();
	void createOneMeshForAllGeometry();
	void setMaterialToMeshBinding(uint32_t materialIndex, uint32_t meshIndex);
	void processMeshGeometry(const aiMesh* assimpMesh);
	void processNode(const aiNode* node, const glm::mat4& transformToModelSpace);
	void processVertex(const aiMesh* assimpMesh, uint32_t vertexOffset);
	void processTriangleIndex(const aiMesh* assimpMesh, uint32_t faceOffset);

	void processMaterials(MaterialModel materialModel);
	void processBlinnPhongMaterials();
	void processPBRMaterials();
	void processBlinnPhongMaterialTextures(const aiMaterial* assimpMaterial, Reference<BlinnPhongMaterial>& material);
	void processBlinnPhongMaterialConstants(const aiMaterial* assimpMaterial, Reference<BlinnPhongMaterial>& material);
	void processPBRMaterialTextures(const aiMaterial* assimpMaterial, Reference<PBRMaterial>& material);
	void processPBRMaterialConstants(const aiMaterial* assimpMaterial, Reference<PBRMaterial>& material);
	Reference<Texture> loadMaterialTexture(const char* textureFilePathRelativeToModel, Texture::TextureSpecification textureSpecification = Texture::TextureSpecification());

private:

	std::string m_modelIdentifier;
	std::string m_filePath;
	std::string m_modelDirectoryPath;

	std::vector<Mesh> m_meshes;

	std::vector<Vertex> m_vertices;
	std::vector<TriangleIndex> m_triangleIndices;

	Reference<VertexBuffer> m_vertexBuffer;
	Reference<IndexBuffer> m_indexBuffer;

	std::vector<Reference<Material>> m_materials;
	std::unordered_map<uint32_t, std::vector<uint32_t>> m_materialToMeshMapping;
	std::unordered_map<std::string, Reference<Texture>> m_textures;

	const aiScene* m_assimpScene;
};