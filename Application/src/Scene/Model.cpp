#include "PCH.h"
#include "Model.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"

static const uint32_t ASSIMP_PREPROCESS_FLAGS =
	aiPostProcessSteps::aiProcess_CalcTangentSpace         | // If not provided by the model, calculate tangent and bitangent vectors for each vertex
	aiPostProcessSteps::aiProcess_JoinIdenticalVertices    | // Get rid of any duplicate vertices in the imported model and record that in the indices, to optimise memory use
	aiPostProcessSteps::aiProcess_Triangulate              | // Ensure triangle primitives are being used in the model
	aiPostProcessSteps::aiProcess_GenNormals               | // If not provided by the model, calculate normals for each vertex
	aiPostProcessSteps::aiProcess_ValidateDataStructure    | // Validate the imported model to check for any issues
	aiPostProcessSteps::aiProcess_RemoveRedundantMaterials | // Remove materials that are not used by any of the meshes
	aiPostProcessSteps::aiProcess_GenUVCoords              | // Convert non UV mapped texture coordinates (like cylinderical mapping) to UV ones
	aiPostProcessSteps::aiProcess_OptimizeMeshes           | // Try and merge meshes together to reduce the number of draw calls
	aiPostProcessSteps::aiProcess_GlobalScale;               // Standardise the scaling of imported models

static glm::mat4 getGLMMat4FromAssimpMat4(const aiMatrix4x4 assimpMat4)
{
	glm::mat4 glmMat4;
	glmMat4[0] = { assimpMat4.a1, assimpMat4.b1, assimpMat4.c1, assimpMat4.d1 };
	glmMat4[1] = { assimpMat4.a2, assimpMat4.b2, assimpMat4.c2, assimpMat4.d2 };
	glmMat4[2] = { assimpMat4.a3, assimpMat4.b3, assimpMat4.c3, assimpMat4.d3 };
	glmMat4[3] = { assimpMat4.a4, assimpMat4.b4, assimpMat4.c4, assimpMat4.d4 };
	return glmMat4;
}

Model::Model(const std::string& filePath, MaterialModel materialModel)
	: m_modelIdentifier(filePath), m_filePath(filePath)
{
	std::filesystem::path path(filePath);
	m_modelDirectoryPath = path.parent_path().string();

	Assimp::Importer importer;
	m_assimpScene = importer.ReadFile(filePath, ASSIMP_PREPROCESS_FLAGS);

	if (!m_assimpScene)
		throw ModelCreationException("Error occured when importing model: " + std::string(importer.GetErrorString()));
	if (m_assimpScene->mFlags & AI_SCENE_FLAGS_VALIDATION_WARNING)
		throw ModelCreationException("Model could not be validated: " + std::string(importer.GetErrorString()));

	Log::trace("Successfully loaded source model file {0}", m_modelIdentifier);

	m_meshes.reserve(m_assimpScene->mNumMeshes);

	processMeshes();
	processModelGraph();
	createBuffers();

	processMaterials(materialModel);

	Log::info("Created model {0}", m_modelIdentifier);
	Log::trace("\tVertices:  {0}", m_vertices.size());
	Log::trace("\tIndices:   {0}", m_triangleIndices.size() * 3u);
	Log::trace("\tMaterials: {0}", m_materials.size());
}

Model::Model(const std::string modelIdentifier, const std::vector<Vertex>& vertices, const std::vector<TriangleIndex>& triangleIndices, const Reference<Material>& material)
	: m_modelIdentifier(modelIdentifier), m_filePath("CREATED_USING_MODEL_FACTORY"), m_modelDirectoryPath("CREATED_USING_MODEL_FACTORY"),
	  m_vertices(vertices), m_triangleIndices(triangleIndices), m_assimpScene(nullptr)
{
	createOneMeshForAllGeometry();
	createBuffers();

	m_materials.push_back(material);
	m_materialToMeshMapping[0] = { 0 };
}

void Model::processMeshes()
{
	for (uint32_t i = 0; i < m_assimpScene->mNumMeshes; i++)
	{
		const aiMesh* assimpMesh = m_assimpScene->mMeshes[i];

		Mesh mesh;
		mesh.vertexCount = assimpMesh->mNumVertices;
		// All meshes have been triangulated by the assimp preprocessor so we can assume all faces will be made up of 3 vertices
		mesh.indexCount = assimpMesh->mNumFaces * 3u;
		mesh.baseVertex = static_cast<uint32_t>(m_vertices.size());
		mesh.baseIndex = static_cast<uint32_t>(m_triangleIndices.size()) * 3u;
		mesh.name = assimpMesh->mName.C_Str();
		
		uint32_t materialIndex = assimpMesh->mMaterialIndex;
		setMaterialToMeshBinding(materialIndex, static_cast<uint32_t>(m_meshes.size()));

		processMeshGeometry(assimpMesh);

		m_meshes.push_back(mesh);
	}
}

void Model::processModelGraph()
{
	glm::mat4 rootTransform(1.0f);
	const aiNode* rootNode = m_assimpScene->mRootNode;
	processNode(rootNode, rootTransform);
}

void Model::createBuffers()
{
	VertexBufferLayout vertexBufferLayout =
	{
		{ ShaderDataType::FLOAT3, "a_position" },
		{ ShaderDataType::FLOAT3, "a_normal" },
		{ ShaderDataType::FLOAT2, "a_textureCoordinates"},
		{ ShaderDataType::FLOAT3, "a_tangent"},
		{ ShaderDataType::FLOAT3, "a_bitangent"}
	};

	m_vertexBuffer = createReference<VertexBuffer>(static_cast<void*>(m_vertices.data()), m_vertices.size() * sizeof(Vertex), vertexBufferLayout);
	m_indexBuffer = createReference<IndexBuffer>(reinterpret_cast<uint32_t*>(m_triangleIndices.data()), static_cast<uint32_t>(m_triangleIndices.size()) * 3u);
}

void Model::createOneMeshForAllGeometry()
{
	Mesh mesh;
	mesh.transform = glm::mat4(1.0f);
	mesh.vertexCount = static_cast<uint32_t>(m_vertices.size());
	mesh.indexCount = static_cast<uint32_t>(m_triangleIndices.size()) * 3u;
	mesh.baseVertex = 0;
	mesh.baseIndex = 0;
	mesh.name = m_modelIdentifier;

	m_meshes.push_back(mesh);
}

void Model::setMaterialToMeshBinding(uint32_t materialIndex, uint32_t meshIndex)
{
	if (m_materialToMeshMapping.find(materialIndex) != m_materialToMeshMapping.end())
		m_materialToMeshMapping.at(materialIndex).push_back(meshIndex);
	else
		m_materialToMeshMapping[materialIndex] = std::vector<uint32_t>({ meshIndex });
}

void Model::processMeshGeometry(const aiMesh* assimpMesh)
{
	for (uint32_t i = 0; i < assimpMesh->mNumVertices; i++)
		processVertex(assimpMesh, i);

	for (uint32_t i = 0; i < assimpMesh->mNumFaces; i++)
		processTriangleIndex(assimpMesh, i);
}

void Model::processNode(const aiNode* node, const glm::mat4& transformToModelSpace)
{
	glm::mat4 localTransform = getGLMMat4FromAssimpMat4(node->mTransformation);
	glm::mat4 transformToModelSpaceUpdated = transformToModelSpace * localTransform;

	for (uint32_t i = 0; i < node->mNumMeshes; i++)
	{
		const uint32_t meshIndex = node->mMeshes[i];
		Mesh& mesh = m_meshes[meshIndex];
		mesh.transform = transformToModelSpaceUpdated;
	}

	for (uint32_t i = 0; i < node->mNumChildren; i++)
		processNode(node->mChildren[i], transformToModelSpaceUpdated);
}

void Model::processVertex(const aiMesh* assimpMesh, uint32_t vertexOffset)
{
	Vertex vertex;
	vertex.position = { assimpMesh->mVertices[vertexOffset].x, assimpMesh->mVertices[vertexOffset].y, assimpMesh->mVertices[vertexOffset].z };

	if (assimpMesh->HasNormals())
		vertex.normal = { assimpMesh->mNormals[vertexOffset].x, assimpMesh->mNormals[vertexOffset].y, assimpMesh->mNormals[vertexOffset].z };

	if (assimpMesh->HasTextureCoords(0))
		vertex.textureCoordinates = { assimpMesh->mTextureCoords[0][vertexOffset].x, assimpMesh->mTextureCoords[0][vertexOffset].y };

	if (assimpMesh->HasTangentsAndBitangents())
	{
		vertex.tangent = { assimpMesh->mTangents[vertexOffset].x, assimpMesh->mTangents[vertexOffset].y, assimpMesh->mTangents[vertexOffset].z };
		vertex.bitangent = { assimpMesh->mBitangents[vertexOffset].x, assimpMesh->mBitangents[vertexOffset].y, assimpMesh->mBitangents[vertexOffset].z };
	}

	m_vertices.push_back(vertex);
}

void Model::processTriangleIndex(const aiMesh* assimpMesh, uint32_t faceOffset)
{
	TriangleIndex triangleIndex =
	{
		assimpMesh->mFaces[faceOffset].mIndices[0],
		assimpMesh->mFaces[faceOffset].mIndices[1],
		assimpMesh->mFaces[faceOffset].mIndices[2]
	};

	m_triangleIndices.push_back(triangleIndex);
}

void Model::processMaterials(MaterialModel materialModel)
{
	switch (materialModel)
	{
	case Model::MaterialModel::BLINN_PHONG:
		processBlinnPhongMaterials();
		break;
	case Model::MaterialModel::PBR:
		processPBRMaterials();
		break;
	default:
		ASSERT_MESSAGE(false, "Unknown materialModel - cannot import material properties of model");
		break;
	}
}

void Model::processBlinnPhongMaterials()
{
	m_materials.reserve(m_assimpScene->mNumMaterials);

	for (uint32_t i = 0; i < m_assimpScene->mNumMaterials; i++)
	{
		const aiMaterial* assimpMaterial = m_assimpScene->mMaterials[i];
		std::string materialName = std::string(assimpMaterial->GetName().C_Str());
		Reference<BlinnPhongMaterial> material = BlinnPhongMaterial::create();

		processBlinnPhongMaterialTextures(assimpMaterial, material);
		processBlinnPhongMaterialConstants(assimpMaterial, material);

		m_materials.push_back(material);

		Log::info("Loaded Blinn-Phong material {0} from model {1}", materialName, m_modelIdentifier);
		Log::trace("\tDiffuse color:    ({0}, {1}, {2}, {3})", material->diffuseColor.r, material->diffuseColor.g, material->diffuseColor.b, material->diffuseColor.a);
		Log::trace("\tSpecular color:   ({0}, {1}, {2})", material->specularColor.r, material->specularColor.g, material->specularColor.b);
		Log::trace("\tShininess:        {0}", material->shininess);
		if (material->diffuseMap)
			Log::trace("\tDiffuse map:      {0}", material->diffuseMap->getTextureSpecification().filePath);
		if (material->specularMap)
			Log::trace("\tSpecular map:     {0}", material->specularMap->getTextureSpecification().filePath);
		if (material->normalMap)
			Log::trace("\tNormal map:       {0}", material->normalMap->getTextureSpecification().filePath);
	}
}

void Model::processPBRMaterials()
{
	m_materials.reserve(m_assimpScene->mNumMaterials);

	for (uint32_t i = 0; i < m_assimpScene->mNumMaterials; i++)
	{
		const aiMaterial* assimpMaterial = m_assimpScene->mMaterials[i];
		std::string materialName = std::string(assimpMaterial->GetName().C_Str());
		Reference<PBRMaterial> material = PBRMaterial::create();

		processPBRMaterialTextures(assimpMaterial, material);
		processPBRMaterialConstants(assimpMaterial, material);

		m_materials.push_back(material);

		Log::info("Loaded PBR material {0} from model {1}", materialName, m_modelIdentifier);
		Log::trace("\tBase color:       ({0}, {1}, {2}, {3})", material->baseColor.r, material->baseColor.g, material->baseColor.b, material->baseColor.a);
		Log::trace("\tRoughness:        {0}", material->roughness);
		Log::trace("\tMetalness:        {0}", material->metalness);
		if (material->baseColorMap)
			Log::trace("\tBase color map:   {0}", material->baseColorMap->getTextureSpecification().filePath);
		if (material->roughnessMap)
			Log::trace("\tRoughness map:    {0}", material->roughnessMap->getTextureSpecification().filePath);
		if (material->metalnessMap)
			Log::trace("\tMetalness map:    {0}", material->metalnessMap->getTextureSpecification().filePath);
		if (material->normalMap)
			Log::trace("\tNormal map:       {0}", material->normalMap->getTextureSpecification().filePath);
	}
}

void Model::processBlinnPhongMaterialTextures(const aiMaterial* assimpMaterial, Reference<BlinnPhongMaterial>& material)
{
	aiString diffuseTextureFilePathRelativeToModel;
	if (assimpMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &diffuseTextureFilePathRelativeToModel) == aiReturn_SUCCESS)
	{
		Texture::TextureSpecification diffuseTextureSpecification;
		diffuseTextureSpecification.SRGB = true;
		material->diffuseMap = loadMaterialTexture(diffuseTextureFilePathRelativeToModel.C_Str(), diffuseTextureSpecification);
	}

	aiString specularTextureFilePathRelativeToModel;
	if (assimpMaterial->GetTexture(aiTextureType_SPECULAR, 0, &specularTextureFilePathRelativeToModel) == aiReturn_SUCCESS)
	{
		Texture::TextureSpecification specularTextureSpecification;
		specularTextureSpecification.SRGB = true;
		material->specularMap = loadMaterialTexture(specularTextureFilePathRelativeToModel.C_Str(), specularTextureSpecification);
	}

	// OBJ fileformats can specify normal maps as bump maps (height maps) so need to account for this when loading normal maps

	aiString normalTextureFilePathRelativeToModel;
	if (assimpMaterial->GetTexture(aiTextureType_NORMALS, 0, &normalTextureFilePathRelativeToModel) == aiReturn_SUCCESS)
		material->normalMap = loadMaterialTexture(normalTextureFilePathRelativeToModel.C_Str());
	else
	{
		aiString heightTextureFilePathRelativeToModel;
		if (assimpMaterial->GetTexture(aiTextureType_HEIGHT, 0, &heightTextureFilePathRelativeToModel) == aiReturn_SUCCESS)
			material->normalMap = loadMaterialTexture(heightTextureFilePathRelativeToModel.C_Str());
	}
}

void Model::processBlinnPhongMaterialConstants(const aiMaterial* assimpMaterial, Reference<BlinnPhongMaterial>& material)
{
	if (!(material->diffuseMap))
	{
		aiColor4D assimpDiffuseColor;
		assimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, assimpDiffuseColor);
		material->diffuseColor = { assimpDiffuseColor.r, assimpDiffuseColor.g, assimpDiffuseColor.b, assimpDiffuseColor.a };
	}

	if (!(material->specularMap))
	{
		aiColor3D assimpSpecularColor;
		assimpMaterial->Get(AI_MATKEY_COLOR_SPECULAR, assimpSpecularColor);
		material->specularColor = { assimpSpecularColor.r, assimpSpecularColor.g, assimpSpecularColor.b };
	}

	float assimpShininess;
	assimpMaterial->Get(AI_MATKEY_SHININESS, assimpShininess);
	material->shininess = assimpShininess;
}

void Model::processPBRMaterialTextures(const aiMaterial* assimpMaterial, Reference<PBRMaterial>& material)
{
	// Depending on the model, the base color map may be a aiTextureType_BASE_COLOR or aiTextureType_DIFFUSE

	aiString baseColorTextureFilePathRelativeToModel;
	if (assimpMaterial->GetTexture(aiTextureType_BASE_COLOR, 0, &baseColorTextureFilePathRelativeToModel) == aiReturn_SUCCESS)
	{
		Texture::TextureSpecification baseColorTextureSpecification;
		baseColorTextureSpecification.SRGB = true;
		material->baseColorMap = loadMaterialTexture(baseColorTextureFilePathRelativeToModel.C_Str(), baseColorTextureSpecification);
	}
	else
	{
		if (assimpMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &baseColorTextureFilePathRelativeToModel) == aiReturn_SUCCESS)
		{
			Texture::TextureSpecification baseColorTextureSpecification;
			baseColorTextureSpecification.SRGB = true;
			material->baseColorMap = loadMaterialTexture(baseColorTextureFilePathRelativeToModel.C_Str(), baseColorTextureSpecification);
		}
	}

	// Roughness map can be found under aiTextureType_SHININESS, but not aiTextureType_DIFFUSE_ROUGHNESS for some reason

	aiString roughnessTextureFilePathRelativeToModel;
	if (assimpMaterial->GetTexture(aiTextureType_SHININESS, 0, &roughnessTextureFilePathRelativeToModel) == aiReturn_SUCCESS)
		material->roughnessMap = loadMaterialTexture(roughnessTextureFilePathRelativeToModel.C_Str());

	aiString metalnessTextureFilePathRelativeToModel;
	if (assimpMaterial->GetTexture(aiTextureType_METALNESS, 0, &metalnessTextureFilePathRelativeToModel) == aiReturn_SUCCESS)
		material->metalnessMap = loadMaterialTexture(metalnessTextureFilePathRelativeToModel.C_Str());

	aiString normalTextureFilePathRelativeToModel;
	if (assimpMaterial->GetTexture(aiTextureType_NORMALS, 0, &normalTextureFilePathRelativeToModel) == aiReturn_SUCCESS)
		material->normalMap = loadMaterialTexture(normalTextureFilePathRelativeToModel.C_Str());
}

void Model::processPBRMaterialConstants(const aiMaterial* assimpMaterial, Reference<PBRMaterial>& material)
{
	if (!(material->baseColorMap))
	{
		aiColor4D assimpBaseColor;
		if (assimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, assimpBaseColor) == aiReturn_SUCCESS)
			material->baseColor = { assimpBaseColor.r, assimpBaseColor.g, assimpBaseColor.b, assimpBaseColor.a };
	}

	if (!(material->roughnessMap))
	{
		float assimpRoughness;
		if (assimpMaterial->Get(AI_MATKEY_SHININESS, assimpRoughness) == aiReturn_SUCCESS)
			material->roughness = 1.0f - glm::sqrt(assimpRoughness / 100.0f);
	}

	if (!(material->metalnessMap))
	{
		float assimpMetalness;
		if (assimpMaterial->Get(AI_MATKEY_REFLECTIVITY, assimpMetalness) == aiReturn_SUCCESS)
			material->metalness = assimpMetalness;
	}
}

Reference<Texture> Model::loadMaterialTexture(const char* textureFilePathRelativeToModel, Texture::TextureSpecification textureSpecification)
{
	std::stringstream ss;
	ss << m_modelDirectoryPath << '/' << textureFilePathRelativeToModel;
	std::string textureFilePath = ss.str();

	Reference<Texture> texture;

	if (m_textures.find(textureFilePath) == m_textures.end())
	{
		textureSpecification.filePath = textureFilePath;
		texture = createReference<Texture>(textureSpecification);

		m_textures[textureFilePath] = texture;
	}
	else
		texture = m_textures.at(textureFilePath);

	return texture;
}
