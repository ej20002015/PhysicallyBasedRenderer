#pragma once
#include "PCH.h"

#include "glm/glm.hpp"

using RendererID = uint32_t;

class RendererUtilities
{
public:

	static void drawIndexed(uint32_t count);
	static void drawIndexedFromVertexOffset(uint32_t count, const void* startOfIndices, uint32_t vertexOffset);

	static void clear();

	static void setClearColor(const glm::vec4& color);

	static void bindDefaultFramebuffer();
};