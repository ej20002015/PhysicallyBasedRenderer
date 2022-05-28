#include "PCH.h"
#include "VertexBuffer.h"

#include "glad/glad.h"

#include "RendererUtilities.h"

VertexBuffer::VertexBuffer(const void* data, size_t size, const VertexBufferLayout& vertexBufferLayout)
	: m_vertexBufferLayout(vertexBufferLayout)
{
	glCreateBuffers(1, &m_rendererID);

	// Just specifying static access frequency for all buffers at the moment
	glNamedBufferData(m_rendererID, size, data, GL_STATIC_DRAW);

	Log::info("Created vertex buffer {0}", m_rendererID);
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &m_rendererID);

	Log::info("Deleted vertex buffer {0}", m_rendererID);
}

void VertexBuffer::bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
	m_vertexBufferLayout.bind();
	
	Log::trace("Bound vertex buffer {0}", m_rendererID);
}
