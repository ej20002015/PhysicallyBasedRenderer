#pragma once
#include "PCH.h"

#include "glm/glm.hpp"
#include "glad/glad.h"

#include "RendererUtilities.h"

class Framebuffer
{
public:

	enum class ColorAttachmentFormat
	{
		RGBA8 = 0,
		RGBA16F
	};

	enum class DepthAttachmentFormat
	{
		NONE = 0,
		DEPTH24STENCIL8,
	};

	struct FramebufferSpecification
	{
		uint32_t width = DEFAULT_WIDTH, height = DEFAULT_HEIGHT;
		ColorAttachmentFormat colorAttachmentFormat = ColorAttachmentFormat::RGBA8;
		DepthAttachmentFormat depthAttachmentFormat = DepthAttachmentFormat::DEPTH24STENCIL8;
		bool resizeWithWindowResizeEvents = true;
		glm::vec4 clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		uint32_t samples = 1;
	};

public:

	Framebuffer() = delete;
	Framebuffer(const FramebufferSpecification& specification);
	~Framebuffer();
	Framebuffer(const Framebuffer&) = delete;

	void bind() const;
	void bindColorAttachment(uint32_t textureSlot = 0) const;
	void bindDepthAttachment(uint32_t textureSlot = 0) const;

	void onWindowResizeEvent(uint32_t width, uint32_t height);

	void clear() const;

	void blitToTargetFramebuffer(Reference<const Framebuffer> target = nullptr);

private:

	void resize(uint32_t width, uint32_t height);

	void createFramebuffer(uint32_t width, uint32_t height);
	void setUpColorAttachment(uint32_t width, uint32_t height);
	void setUpDepthAttachment(uint32_t width, uint32_t height);

	GLenum getOpenGLColorAttachmentSizedInternalFormat() const;
	GLenum getOpenGLDepthAttachmentSizedInternalFormat() const;
	GLenum getOpenGLDepthAttachmentType() const;

	void deleteFramebuffer() const;

private:

	RendererID m_rendererID = 0;
	RendererID m_colorAttachmentRendererID = 0, m_depthAttachmentRendererID = 0;
	FramebufferSpecification m_specification;
};