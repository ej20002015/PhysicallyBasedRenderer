#include "PCH.h"
#include "Renderer.h"

#include "glad/glad.h"

RendererID Renderer::s_vertexArrayRendererID = 0;

Renderer::RendererType Renderer::s_currentRendererType = Renderer::RendererType::BLINN_PHONG;

RendererImplementation* Renderer::s_currentRendererImplementation = nullptr;
Unique<BlinnPhongRendererImplementation> Renderer::s_blinnPhongRendererImplementation;
Unique<PBRRendererImplementation> Renderer::s_PBRRendererImplementation;

static void openGLErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:
		Log::critical("OpenGL Error [{0}]: {1}", type, message);
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		Log::error("OpenGL Error [{0}]: {1}", type, message);
		break;
	case GL_DEBUG_SEVERITY_LOW:
		Log::warn("OpenGL Error [{0}]: {1}", type, message);
		break;
	default:
		break;
	}
}

void Renderer::init()
{
	// Set up error callback

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(openGLErrorCallback, nullptr);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glEnable(GL_MULTISAMPLE);

	// Enable back face culling

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	// Set up blending

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Create the one global Vertex Array Object (VAO).
	// This VAO is necessary for OpenGL to work but adds no real functionality or performance.
	// As such, one global VAO is used for everything

	glCreateVertexArrays(1, &s_vertexArrayRendererID);
	glBindVertexArray(s_vertexArrayRendererID);

	s_blinnPhongRendererImplementation = createUnique<BlinnPhongRendererImplementation>();
	s_PBRRendererImplementation = createUnique<PBRRendererImplementation>();

	s_currentRendererImplementation = static_cast<RendererImplementation*>(s_blinnPhongRendererImplementation.get());

	Log::info("Renderer initialised");
}

void Renderer::shutdown()
{
	glDeleteVertexArrays(1, &s_vertexArrayRendererID);
}

void Renderer::onWindowResizeEvent(uint32_t width, uint32_t height)
{
	glViewport(0, 0, width, height);

	// Not just propogating the resize to the current renderer implementation
	// as even the renderer not currently being used should be updating its state
	// - if it doesn't, then when it gets switched in there may be issues

	s_blinnPhongRendererImplementation->onWindowResizeEvent(width, height);
	s_PBRRendererImplementation->onWindowResizeEvent(width, height);
}

void Renderer::setRendererType(RendererType rendererType)
{
	switch (rendererType)
	{
	case Renderer::RendererType::BLINN_PHONG:
		s_currentRendererImplementation = static_cast<RendererImplementation*>(s_blinnPhongRendererImplementation.get());
		Log::info("Switched to Blinn-Phong Renderer Implementation");
		break;
	case Renderer::RendererType::PBR:
		s_currentRendererImplementation = static_cast<RendererImplementation*>(s_PBRRendererImplementation.get());
		Log::info("Switched to PBR Implementation");
		break;
	default:
		ASSERT_MESSAGE(false, "Unknown RendererType");
		break;
	}

	Log::info("Switched renderer type");
}

void Renderer::beginScene(const Camera& camera, const std::vector<Reference<PointLight>>& pointLights)
{
	s_currentRendererImplementation->beginScene(camera, pointLights);
}

void Renderer::endScene(float exposureLevel)
{
	s_currentRendererImplementation->endScene(exposureLevel);
}

void Renderer::drawScene(const Reference<Scene>& scene, const Camera& camera)
{
	s_currentRendererImplementation->drawScene(scene, camera);
}

void Renderer::drawModel(Reference<Model> model, const glm::mat4& transform)
{
	s_currentRendererImplementation->drawModel(model, transform);
}

void Renderer::clear()
{
	RendererUtilities::clear();
}

void Renderer::setClearColor(const glm::vec4& color)
{
	RendererUtilities::setClearColor(color);
}

void Renderer::bindDefaultFramebuffer()
{
	RendererUtilities::bindDefaultFramebuffer();
}
