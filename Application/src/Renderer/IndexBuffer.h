#pragma once
#include "PCH.h"

#include "RendererUtilities.h"

class IndexBuffer
{
public:

	IndexBuffer() = delete;
	IndexBuffer(const uint32_t* data, uint32_t count);
	~IndexBuffer();
	IndexBuffer(const IndexBuffer&) = delete;

	void bind() const;

	uint32_t getCount() const { return m_count; }

private:

	RendererID m_rendererID;
	uint32_t m_count = 0;
};