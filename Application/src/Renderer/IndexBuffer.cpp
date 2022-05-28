#include "PCH.h"
#include "IndexBuffer.h"

#include "glad/glad.h"

IndexBuffer::IndexBuffer(const uint32_t* data, uint32_t count)
	: m_count(count)
{
	glCreateBuffers(1, &m_rendererID);

	glNamedBufferData(m_rendererID, sizeof(uint32_t) * count, static_cast<const void*>(data), GL_STATIC_DRAW);

	Log::info("Created index buffer {0}", m_rendererID);
}

IndexBuffer::~IndexBuffer()
{
	glDeleteBuffers(1, &m_rendererID);

	Log::info("Deleted index buffer {0}", m_rendererID);
}

void IndexBuffer::bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);

	Log::trace("Bound index buffer {0}", m_rendererID);
}
