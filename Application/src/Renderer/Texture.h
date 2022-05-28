#pragma once
#include "PCH.h"

#include "glad/glad.h"

#include "RendererUtilities.h"

class Texture
{
public:

	struct TextureCreationException : public std::exception
	{
		std::string errorMessage;

		TextureCreationException(const std::string errorMessage)
			: errorMessage("TextureCreationException Occured - Texture Not Created: " + errorMessage) {}

		const char* what() const override
		{
			return errorMessage.c_str();
		}
	};

	enum class WrappingMode
	{
		REPEAT = 0,
		MIRRORED_REPEAT,
		CLAMP_TO_EDGE,
		CLAMP_TO_BORDER
	};

	enum class Filter
	{
		LINEAR = 0,
		NEAREST
	};

	struct TextureSpecification
	{
		std::string filePath;
		WrappingMode wrappingMode = WrappingMode::REPEAT;
		Filter minFilter = Filter::LINEAR;
		Filter magFilter = Filter::LINEAR;
		bool SRGB = false;
	};

public:

	Texture() = delete;
	Texture(const TextureSpecification& specification);
	Texture(void* imageData, uint32_t width, uint32_t height, uint32_t channels);
	~Texture();
	Texture(const Texture&) = delete;

	void bind(uint32_t textureSlot = 0) const;

	uint32_t getWidth() const { return m_width; }
	uint32_t getHeight() const { return m_height; }

	const TextureSpecification& getTextureSpecification() const { return m_specification; }

private:

	void* loadImageData();
	void setUpTexture(void* imageData);
	void freeImageData(void* imageData) const;

	std::tuple<GLenum, GLenum> getOpenGLInternalFormats() const;

	void setUpTextureProperties();

	GLenum getOpenGLWrappingMode() const;
	GLenum getOpenGLMinFilter() const;
	GLenum getOpenGLMagFilter() const;

	uint32_t calculateNumberOfMipMapLevels() const;

private:

	RendererID m_rendererID;

	uint32_t m_width, m_height;
	uint32_t m_channels;
	TextureSpecification m_specification;
};
