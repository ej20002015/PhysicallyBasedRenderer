#pragma once
#include "PCH.h"

#include "glad/glad.h"

#include "RendererUtilities.h"

class IndividualShader;

class Shader
{
public:

	struct ShaderCreationException : public std::exception
	{
		std::string errorMessage;

		ShaderCreationException(const std::string errorMessage)
			: errorMessage("ShaderCreationException Occured - Shader Not Created: " + errorMessage) {}

		const char* what() const noexcept override
		{
			return errorMessage.c_str();
		}
	};

public:

	Shader() = delete;
	Shader(const std::initializer_list<std::string>& individualShaderSourceFilePaths);
	~Shader();
	Shader(const Shader&) = delete;

	void bind() const;

	// Methods to set uniforms within the shader
	// Data types correspond to the possible values of ShaderDataType

	void setUniformToValue(const std::string uniformIdentifier, float value);
	void setUniformToValue(const std::string uniformIdentifier, const glm::vec2& value);
	void setUniformToValue(const std::string uniformIdentifier, const glm::vec3& value);
	void setUniformToValue(const std::string uniformIdentifier, const glm::vec4& value);

	void setUniformToValue(const std::string uniformIdentifier, const uint32_t value);
	void setUniformToValue(const std::string uniformIdentifier, const glm::uvec2& value);
	void setUniformToValue(const std::string uniformIdentifier, const glm::uvec3& value);
	void setUniformToValue(const std::string uniformIdentifier, const glm::uvec4& value);

	void setUniformToValue(const std::string uniformIdentifier, const int32_t value);
	void setUniformToValue(const std::string uniformIdentifier, const glm::ivec2& value);
	void setUniformToValue(const std::string uniformIdentifier, const glm::ivec3& value);
	void setUniformToValue(const std::string uniformIdentifier, const glm::ivec4& value);

	void setUniformToValue(const std::string uniformIdentifier, const glm::mat2& value);
	void setUniformToValue(const std::string uniformIdentifier, const glm::mat3& value);
	void setUniformToValue(const std::string uniformIdentifier, const glm::mat4& value);

private:

	void retrieveAndCompileIndividualShaders();
	void linkProgram();
	void cleanUpIndividualShaders();

	void processProgramLinkingErrors();

	int32_t getUniformLocation(const std::string uniformIdentifier);

private:

	RendererID m_rendererID;
	std::vector<std::string> m_individualShaderSourceFilePaths;
	std::vector<IndividualShader*> m_individualShaders;

	std::unordered_map<std::string, int32_t> m_uniformLocations;
};

class IndividualShader
{
public:

	enum class IndividualShaderType
	{
		NONE = 0,
		VERTEX,
		FRAGMENT,
		COMPUTE
	};

public:

	IndividualShader() = delete;
	IndividualShader(const std::string& filePath);
	~IndividualShader();
	IndividualShader(const IndividualShader&) = delete;

	RendererID getRendererID() const { return m_rendererID; }

private:

	void retrieveTypeAndSource();
	void compileIndividualShader();
	void processCompilationErrors();

	IndividualShaderType getShaderTypeFromFileExtension(const std::string& extension) const;
	GLenum convertShaderTypeToOpenGLShaderType(IndividualShaderType individualShaderType) const;

private:

	RendererID m_rendererID;
	IndividualShaderType m_individualShaderType;
	std::string m_filePath;
	std::string m_source;
};