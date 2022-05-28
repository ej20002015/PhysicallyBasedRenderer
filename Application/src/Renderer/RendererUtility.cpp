#include "PCH.h"
#include "RendererUtilities.h"

#include "glad/glad.h"

void RendererUtilities::drawIndexed(uint32_t count)
{
	// Just using GL_TRIANGLES as the render primitive for now
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);

	Log::trace("Drew {0} indices", count);
}

void RendererUtilities::drawIndexedFromVertexOffset(uint32_t count, const void* startOfIndices, uint32_t vertexOffset)
{
	glDrawElementsBaseVertex(GL_TRIANGLES, count, GL_UNSIGNED_INT, startOfIndices, static_cast<int32_t>(vertexOffset));

	Log::trace("Drew {0} indices, from index {1}, with vertex offset {2}", count, startOfIndices, vertexOffset);
}

void RendererUtilities::clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void RendererUtilities::setClearColor(const glm::vec4& color)
{
	glClearColor(color.r, color.g, color.b, color.a);
}

void RendererUtilities::bindDefaultFramebuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Log::trace("Bound the default framebuffer");
}


