#include "PCH.h"
#include "Shader.h"

#include "glm/gtc/type_ptr.hpp"

Shader::Shader(const std::initializer_list<std::string>& individualShaderSourceFilePaths)
	: m_individualShaderSourceFilePaths(individualShaderSourceFilePaths)
{
	m_rendererID = glCreateProgram();

	try
	{
		retrieveAndCompileIndividualShaders();
		linkProgram();
		cleanUpIndividualShaders();

		Log::info("Created shader {0}", m_rendererID);
	}
	catch (const ShaderCreationException& e)
	{
		Log::error(e.what());
	}
}

Shader::~Shader()
{
	glDeleteProgram(m_rendererID);

	Log::info("Deleted shader {0}", m_rendererID);
}

void Shader::bind() const
{
	glUseProgram(m_rendererID);

	Log::trace("Bound shader {0}", m_rendererID);
}

void Shader::setUniformToValue(const std::string uniformIdentifier, float value)
{
	glUniform1f(getUniformLocation(uniformIdentifier), value);

	Log::trace("Set uniform '{0}' in shader {1} to value = {2}", uniformIdentifier, m_rendererID, value);
}

void Shader::setUniformToValue(const std::string uniformIdentifier, const glm::vec2& value)
{
	glUniform2f(getUniformLocation(uniformIdentifier), value.x, value.y);

	Log::trace("Set uniform '{0}' in shader {1} to value of type glm::vec2", uniformIdentifier, m_rendererID);
}

void Shader::setUniformToValue(const std::string uniformIdentifier, const glm::vec3& value)
{
	glUniform3f(getUniformLocation(uniformIdentifier), value.x, value.y, value.z);

	Log::trace("Set uniform '{0}' in shader {1} to value of type glm::vec3", uniformIdentifier, m_rendererID);
}

void Shader::setUniformToValue(const std::string uniformIdentifier, const glm::vec4& value)
{
	glUniform4f(getUniformLocation(uniformIdentifier), value.x, value.y, value.z, value.w);

	Log::trace("Set uniform '{0}' in shader {1} to value of type glm::vec4", uniformIdentifier, m_rendererID);
}

void Shader::setUniformToValue(const std::string uniformIdentifier, const uint32_t value)
{
	glUniform1ui(getUniformLocation(uniformIdentifier), value);

	Log::trace("Set uniform '{0}' in shader {1} to value = {2}", uniformIdentifier, m_rendererID, value);
}

void Shader::setUniformToValue(const std::string uniformIdentifier, const glm::uvec2& value)
{
	glUniform2ui(getUniformLocation(uniformIdentifier), value.x, value.y);

	Log::trace("Set uniform '{0}' in shader {1} to value of type glm::uvec2", uniformIdentifier, m_rendererID);
}

void Shader::setUniformToValue(const std::string uniformIdentifier, const glm::uvec3& value)
{
	glUniform3ui(getUniformLocation(uniformIdentifier), value.x, value.y, value.z);

	Log::trace("Set uniform '{0}' in shader {1} to value of type glm::uvec3", uniformIdentifier, m_rendererID);
}

void Shader::setUniformToValue(const std::string uniformIdentifier, const glm::uvec4& value)
{
	glUniform4ui(getUniformLocation(uniformIdentifier), value.x, value.y, value.z, value.w);

	Log::trace("Set uniform '{0}' in shader {1} to value of type glm::uvec4", uniformIdentifier, m_rendererID);
}

void Shader::setUniformToValue(const std::string uniformIdentifier, const int32_t value)
{
	glUniform1i(getUniformLocation(uniformIdentifier), value);

	Log::trace("Set uniform '{0}' in shader {1} to value = {2}", uniformIdentifier, m_rendererID, value);
}

void Shader::setUniformToValue(const std::string uniformIdentifier, const glm::ivec2& value)
{
	glUniform2i(getUniformLocation(uniformIdentifier), value.x, value.y);

	Log::trace("Set uniform '{0}' in shader {1} to value of type glm::ivec2", uniformIdentifier, m_rendererID);
}

void Shader::setUniformToValue(const std::string uniformIdentifier, const glm::ivec3& value)
{
	glUniform3i(getUniformLocation(uniformIdentifier), value.x, value.y, value.z);

	Log::trace("Set uniform '{0}' in shader {1} to value of type glm::ivec3", uniformIdentifier, m_rendererID);
}

void Shader::setUniformToValue(const std::string uniformIdentifier, const glm::ivec4& value)
{
	glUniform4i(getUniformLocation(uniformIdentifier), value.x, value.y, value.z, value.w);

	Log::trace("Set uniform '{0}' in shader {1} to value of type glm::ivec4", uniformIdentifier, m_rendererID);
}

void Shader::setUniformToValue(const std::string uniformIdentifier, const glm::mat2& value)
{
	glUniformMatrix2fv(getUniformLocation(uniformIdentifier), 1, false, glm::value_ptr(value));

	Log::trace("Set uniform '{0}' in shader {1} to value of type glm::mat2", uniformIdentifier, m_rendererID);
}

void Shader::setUniformToValue(const std::string uniformIdentifier, const glm::mat3& value)
{
	glUniformMatrix3fv(getUniformLocation(uniformIdentifier), 1, false, glm::value_ptr(value));

	Log::trace("Set uniform '{0}' in shader {1} to value of type glm::mat3", uniformIdentifier, m_rendererID);
}

void Shader::setUniformToValue(const std::string uniformIdentifier, const glm::mat4& value)
{
	glUniformMatrix4fv(getUniformLocation(uniformIdentifier), 1, false, glm::value_ptr(value));

	Log::trace("Set uniform '{0}' in shader {1} to value of type glm::mat4", uniformIdentifier, m_rendererID);
}

void Shader::retrieveAndCompileIndividualShaders()
{
	for (const std::string& individualShaderSourceFilePath : m_individualShaderSourceFilePaths)
	{
		IndividualShader* individualShader = new IndividualShader(individualShaderSourceFilePath);
		m_individualShaders.push_back(individualShader);
	}
}

void Shader::linkProgram()
{
	for (IndividualShader* individualShader : m_individualShaders)
		glAttachShader(m_rendererID, individualShader->getRendererID());

	glLinkProgram(m_rendererID);

	processProgramLinkingErrors();
}

void Shader::cleanUpIndividualShaders()
{
	for (IndividualShader* individualShader : m_individualShaders)
	{
		glDetachShader(m_rendererID, individualShader->getRendererID());
		delete individualShader;
	}
}

void Shader::processProgramLinkingErrors()
{
	GLint isLinked;
	glGetProgramiv(m_rendererID, GL_LINK_STATUS, &isLinked);

	if (isLinked == GL_FALSE)
	{
		// Reserve a buffer for the linking log message

		GLint logLength = 0;
		glGetProgramiv(m_rendererID, GL_INFO_LOG_LENGTH, &logLength);
		char* logMessage = new char[logLength * sizeof(char)];

		// Retrieve linking log message

		glGetProgramInfoLog(m_rendererID, logLength * sizeof(char), &logLength, logMessage);
		ASSERT_MESSAGE(false, "Failed to link shader {0}: {1}", m_rendererID, logMessage);
		delete[] logMessage;
	}
}

int32_t Shader::getUniformLocation(const std::string uniformIdentifier)
{
	auto it = m_uniformLocations.find(uniformIdentifier);

	if (it != m_uniformLocations.end())
		return it->second;
	else
	{
		int32_t location = glGetUniformLocation(m_rendererID, uniformIdentifier.c_str());
		ASSERT_MESSAGE(location != -1, "Can't find uniform '{0}' in program {1}", uniformIdentifier, m_rendererID);
		m_uniformLocations[uniformIdentifier] = location;
		return location;
	}
}

IndividualShader::IndividualShader(const std::string& filePath)
	: m_filePath(filePath)
{
	retrieveTypeAndSource();
	compileIndividualShader();

	Log::info("Successfully created and compiled individual shader {0} with ID = {1}", m_filePath, m_rendererID);
}

IndividualShader::~IndividualShader()
{
	glDeleteShader(m_rendererID);
}

void IndividualShader::retrieveTypeAndSource()
{
	// Determine the individual shader type

	std::filesystem::path path = m_filePath;
	std::string fileExtension = path.extension().string();
	m_individualShaderType = getShaderTypeFromFileExtension(fileExtension);

	// Retrieve source code from the file

	std::stringstream buffer;

	std::ifstream inputFileStream(m_filePath);
	if (inputFileStream.fail())
		throw Shader::ShaderCreationException("Couldn't open file " + m_filePath);

	buffer << inputFileStream.rdbuf();
	inputFileStream.close();

	m_source = buffer.str();

	Log::trace("Retrieved individual shader source and type from {0}", m_filePath);
}

void IndividualShader::compileIndividualShader()
{
	const char* source = m_source.c_str();

	GLenum openGLShaderType = convertShaderTypeToOpenGLShaderType(m_individualShaderType);
	m_rendererID = glCreateShader(openGLShaderType);

	glShaderSource(m_rendererID, 1, &source, nullptr);
	glCompileShader(m_rendererID);

	processCompilationErrors();
}

void IndividualShader::processCompilationErrors()
{
	int32_t compilationStatus;
	glGetShaderiv(m_rendererID, GL_COMPILE_STATUS, &compilationStatus);

	if (!compilationStatus)
	{
		// Reserve a buffer for the compilation log message

		int32_t logLength;
		glGetShaderiv(m_rendererID, GL_INFO_LOG_LENGTH, &logLength);
		char* logMessage = new char[logLength * sizeof(char)];

		// Retrieve compilation log message

		glGetShaderInfoLog(m_rendererID, logLength * sizeof(char), &logLength, logMessage);
		ASSERT_MESSAGE(false, "Failed to compile individual shader {0}: {1}", m_rendererID, logMessage);
		delete[] logMessage;
	}
}

IndividualShader::IndividualShaderType IndividualShader::getShaderTypeFromFileExtension(const std::string& extension) const
{
	if (extension == ".vert")
		return IndividualShaderType::VERTEX;
	else if (extension == ".frag")
		return IndividualShaderType::FRAGMENT;
	else if (extension == ".comp")
		return IndividualShaderType::COMPUTE;
	else
	{
		ASSERT_MESSAGE(false, "File extension does not match a known IndividualShaderType");
		return IndividualShaderType::NONE;
	}
}

GLenum IndividualShader::convertShaderTypeToOpenGLShaderType(IndividualShaderType individualShaderType) const
{
	switch (individualShaderType)
	{
	case IndividualShaderType::VERTEX:   return GL_VERTEX_SHADER;   break;
	case IndividualShaderType::FRAGMENT: return GL_FRAGMENT_SHADER; break;
	case IndividualShaderType::COMPUTE:  return GL_COMPUTE_SHADER;  break;
	default:
		ASSERT_MESSAGE(false, "individualShaderType cannot be converted to an OpenGL Shader Type");
		return 0;
		break;
	}
}
