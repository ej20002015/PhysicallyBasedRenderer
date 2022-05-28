#include "PCH.h"
#include "Framebuffer.h"

#include "Core/Application.h"

Framebuffer::Framebuffer(const FramebufferSpecification& specification)
	: m_specification(specification)
{
	createFramebuffer(m_specification.width, m_specification.height);

	Log::info("Created framebuffer {0}", m_rendererID);
}

Framebuffer::~Framebuffer()
{
	deleteFramebuffer();

	Log::info("Deleted framebuffer {0}", m_rendererID);
}

void Framebuffer::bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);
	glViewport(0, 0, m_specification.width, m_specification.height);

	Log::trace("Bound framebuffer {0}", m_rendererID);
}

void Framebuffer::bindColorAttachment(uint32_t textureSlot) const
{
	glBindTextureUnit(textureSlot, m_colorAttachmentRendererID);

	Log::trace("Bound color attachment {0} of framebuffer {1}, to texture slot {2}", m_colorAttachmentRendererID, m_rendererID, textureSlot);
}

void Framebuffer::bindDepthAttachment(uint32_t textureSlot) const
{
	glBindTextureUnit(textureSlot, m_depthAttachmentRendererID);

	Log::trace("Bound depth attachment {0} of framebuffer {1}, to texture slot {2}", m_depthAttachmentRendererID, m_rendererID, textureSlot);
}

void Framebuffer::onWindowResizeEvent(uint32_t width, uint32_t height)
{
	if (m_specification.resizeWithWindowResizeEvents)
	{
		resize(width, height);
		Log::trace("Resized framebuffer {0} to ({1}, {2})", m_rendererID, width, height);
	}
}

void Framebuffer::clear() const
{
	bind();

	RendererUtilities::setClearColor(m_specification.clearColor);
	RendererUtilities::clear();

	Log::trace("Cleared framebuffer {0}", m_rendererID);
}

void Framebuffer::blitToTargetFramebuffer(Reference<const Framebuffer> target)
{
	// Will blit to the default framebuffer if target is a nullptr

	RendererID targetFramebufferRendererID = 0;
	uint32_t targetFramebufferWidth = Application::getWindow().getWidth();
	uint32_t targetFramebufferHeight = Application::getWindow().getHeight();
	if (target)
	{
		targetFramebufferRendererID = target->m_rendererID;
		targetFramebufferWidth = target->m_specification.width;
		targetFramebufferHeight = target->m_specification.height;
	}

	glBlitNamedFramebuffer(
		m_rendererID, 
		targetFramebufferRendererID,
		0, 0, m_specification.width, m_specification.height,
		0, 0, targetFramebufferWidth, targetFramebufferHeight,
		GL_COLOR_BUFFER_BIT,
		GL_LINEAR
	);

	Log::trace("Blitted color attachment {0} of framebuffer {1}, to framebuffer {2}", m_colorAttachmentRendererID, m_rendererID, targetFramebufferRendererID);
}

void Framebuffer::resize(uint32_t width, uint32_t height)
{
	// Framebuffer has previously been created, so we need to delete it 
	// from the GPU before recreating it with a new size

	deleteFramebuffer();
	
	createFramebuffer(width, height);

	m_specification.width = width;
	m_specification.height = height;
}

void Framebuffer::createFramebuffer(uint32_t width, uint32_t height)
{
	glCreateFramebuffers(1, &m_rendererID);

	glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	setUpColorAttachment(width, height);
	setUpDepthAttachment(width, height);

	ASSERT_MESSAGE(glCheckNamedFramebufferStatus(m_rendererID, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer {0} is not complete / has not been created properly", m_rendererID);
}

void Framebuffer::setUpColorAttachment(uint32_t width, uint32_t height)
{
	GLenum colorAttachmentSizedInternalFormat = getOpenGLColorAttachmentSizedInternalFormat();
	
	if (m_specification.samples > 1)
	{
		glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_colorAttachmentRendererID);
		glTextureStorage2DMultisample(m_colorAttachmentRendererID, m_specification.samples, colorAttachmentSizedInternalFormat, width, height, false);
	}
	else
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &m_colorAttachmentRendererID);
		glTextureStorage2D(m_colorAttachmentRendererID, 1, colorAttachmentSizedInternalFormat, width, height);
	}

	glNamedFramebufferTexture(m_rendererID, GL_COLOR_ATTACHMENT0, m_colorAttachmentRendererID, 0);
}

void Framebuffer::setUpDepthAttachment(uint32_t width, uint32_t height)
{
	if (m_specification.depthAttachmentFormat == DepthAttachmentFormat::NONE)
		return;

	GLenum depthAttachmentSizedInternalFormat = getOpenGLDepthAttachmentSizedInternalFormat();

	if (m_specification.samples > 1)
	{
		glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_depthAttachmentRendererID);
		glTextureStorage2DMultisample(m_depthAttachmentRendererID, m_specification.samples, depthAttachmentSizedInternalFormat, width, height, false);
	}
	else
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &m_depthAttachmentRendererID);
		glTextureStorage2D(m_depthAttachmentRendererID, 1, depthAttachmentSizedInternalFormat, width, height);
	}

	GLenum depthAttachmentType = getOpenGLDepthAttachmentType();

	glNamedFramebufferTexture(m_rendererID, depthAttachmentType, m_depthAttachmentRendererID, 0);
}

GLenum Framebuffer::getOpenGLColorAttachmentSizedInternalFormat() const
{
	switch (m_specification.colorAttachmentFormat)
	{
	case ColorAttachmentFormat::RGBA8:
		return GL_RGBA8;
		break;
	case ColorAttachmentFormat::RGBA16F:
		return GL_RGBA16F;
		break;
	default:
		ASSERT_MESSAGE(false, "Cannot get OpenGL equivalent of color attachment format");
		return 0;
		break;
	}
}

GLenum Framebuffer::getOpenGLDepthAttachmentSizedInternalFormat() const
{
	switch (m_specification.depthAttachmentFormat)
	{
	case DepthAttachmentFormat::DEPTH24STENCIL8:
		return GL_DEPTH24_STENCIL8;
		break;
	default:
		ASSERT_MESSAGE(false, "Cannot get OpenGL equivalent of depth attachment format");
		return 0;
		break;
	}
}

GLenum Framebuffer::getOpenGLDepthAttachmentType() const
{
	switch (m_specification.depthAttachmentFormat)
	{
	case DepthAttachmentFormat::DEPTH24STENCIL8:
		return GL_DEPTH_STENCIL_ATTACHMENT;
		break;
	default:
		ASSERT_MESSAGE(false, "Cannot get OpenGL depth attachment type from format");
		return 0;
		break;
	}
}

void Framebuffer::deleteFramebuffer() const
{
	glDeleteFramebuffers(1, &m_rendererID);
	glDeleteTextures(1, &m_colorAttachmentRendererID);
	glDeleteTextures(1, &m_depthAttachmentRendererID);
}
