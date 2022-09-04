#include "PCH.h"
#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#if defined(__GNUC__)
	// GNU has SIMD issues with STBI so it needs to be disabled
	#define STBI_NO_SIMD
#endif
#include "stb_image.h"
#include "glm/glm.hpp"

Texture::Texture(const TextureSpecification& specification)
	: m_specification(specification)
{
	try
	{
		void* imageData = loadImageData();
		setUpTexture(imageData);
		freeImageData(imageData);

		Log::info("Created texture {0} with RendererID {1}", m_specification.filePath, m_rendererID);
	}
	catch (const TextureCreationException& e)
	{
		Log::error(e.what());
	}
}

Texture::Texture(void* imageData, uint32_t width, uint32_t height, uint32_t channels)
{
	m_specification.filePath = "IMAGE_DATA_NOT_FROM_FILE";

	m_width = width;
	m_height = height;
	m_channels = channels;

	setUpTexture(imageData);
	Log::info("Created texture {0} with RendererID {1}", m_specification.filePath, m_rendererID);
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_rendererID);

	Log::info("Deleted texture {0} with RendererID {1}", m_specification.filePath, m_rendererID);
}

void Texture::bind(uint32_t textureSlot) const
{
	glBindTextureUnit(textureSlot, m_rendererID);

	Log::trace("Bound texture {0} with RendererID {1}, to texture slot {2}", m_specification.filePath, m_rendererID, textureSlot);
}

void* Texture::loadImageData()
{
	int32_t width, height, channels;

	stbi_set_flip_vertically_on_load(true);

	void* imageData = reinterpret_cast<void*>(stbi_load(m_specification.filePath.c_str(), &width, &height, &channels, 0));

	if (!imageData)
		throw TextureCreationException("Could not load image " + m_specification.filePath + ": " + stbi_failure_reason());

	m_width = static_cast<uint32_t>(width);
	m_height = static_cast<uint32_t>(height);
	m_channels = static_cast<uint32_t>(channels);

	Log::trace("Loaded image data from {0}", m_specification.filePath);

	return imageData;
}

void Texture::setUpTexture(void* imageData)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererID);

	uint32_t mipMapLevels = calculateNumberOfMipMapLevels();

	const auto [internalFormat, sizedInternalFormat] = getOpenGLInternalFormats();
	glTextureStorage2D(m_rendererID, mipMapLevels, sizedInternalFormat, m_width, m_height);
	glTextureSubImage2D(m_rendererID, 0, 0, 0, m_width, m_height, internalFormat, GL_UNSIGNED_BYTE, imageData);

	glGenerateTextureMipmap(m_rendererID);

	setUpTextureProperties();
}

void Texture::freeImageData(void* imageData) const
{
	stbi_image_free(imageData);
}

std::tuple<GLenum, GLenum> Texture::getOpenGLInternalFormats() const
{
	switch (m_channels)
	{
	case 4:
		if (m_specification.SRGB)
			return { GL_RGBA, GL_SRGB8_ALPHA8 };
		else
			return { GL_RGBA, GL_RGBA8 };
		break;
	case 3:
		if (m_specification.SRGB)
			return { GL_RGB, GL_SRGB8 };
		else
			return { GL_RGB, GL_RGB8 };
		break;
	case 2:
		return { GL_RG, GL_RG8 };
		break;
	case 1:
		return { GL_RED, GL_R8 };
	default:
		ASSERT_MESSAGE(false, "Unsupported number of texture channels");
		return { 0, 0 };
		break;
	}
}

void Texture::setUpTextureProperties()
{
	GLenum wrappingMode = getOpenGLWrappingMode();
	GLenum minFilter = getOpenGLMinFilter();
	GLenum magFilter = getOpenGLMagFilter();

	glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_S, wrappingMode);
	glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_T, wrappingMode);
	glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, minFilter);
	glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, magFilter);
	//glTextureParameterf(m_rendererID, GL_TEXTURE_MAX_ANISOTROPY, 16.0f);
}

GLenum Texture::getOpenGLWrappingMode() const
{
	switch (m_specification.wrappingMode)
	{
	case WrappingMode::REPEAT:
		return GL_REPEAT;
		break;
	case WrappingMode::MIRRORED_REPEAT:
		return GL_MIRRORED_REPEAT;
		break;
	case WrappingMode::CLAMP_TO_EDGE:
		return GL_CLAMP_TO_EDGE;
		break;
	case WrappingMode::CLAMP_TO_BORDER:
		return GL_CLAMP_TO_BORDER;
		break;
	default:
		ASSERT_MESSAGE(false, "Cannot get OpenGL equivalent of wrapping mode");
		return 0;
		break;
	}
}

GLenum Texture::getOpenGLMinFilter() const
{
	switch (m_specification.minFilter)
	{
	case Filter::LINEAR:
		return GL_LINEAR_MIPMAP_LINEAR;
		break;
	case Filter::NEAREST:
		return GL_NEAREST;
		break;
	default:
		ASSERT_MESSAGE(false, "Cannot get OpenGL equivalent of min filter");
		return 0;
		break;
	}
}

GLenum Texture::getOpenGLMagFilter() const
{
	switch (m_specification.magFilter)
	{
	case Filter::LINEAR:
		return GL_LINEAR;
		break;
	case Filter::NEAREST:
		return GL_NEAREST;
		break;
	default:
		ASSERT_MESSAGE(false, "Cannot get OpenGL equivalent of mag filter");
		return 0;
		break;
	}
}

uint32_t Texture::calculateNumberOfMipMapLevels() const
{
	uint32_t levels = 1;

	uint32_t maxDimension = glm::max(m_width, m_height);

	while ((maxDimension = maxDimension >> 1))
		levels++;

	return levels;
}
