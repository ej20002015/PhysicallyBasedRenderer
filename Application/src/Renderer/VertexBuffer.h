#pragma once
#include "PCH.h"

#include "VertexBufferLayout.h"

#include "RendererUtilities.h"

class VertexBuffer
{
public:

	VertexBuffer() = delete;
	VertexBuffer(const void* data, size_t size, const VertexBufferLayout& vertexBufferLayout);
	~VertexBuffer();
	VertexBuffer(const VertexBuffer&) = delete;

	void bind() const;

private:

	RendererID m_rendererID;
	VertexBufferLayout m_vertexBufferLayout;
};