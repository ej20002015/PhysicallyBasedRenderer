#include "PCH.h"
#include "Window.h"

#include "glad/glad.h"

static void glfwErrorCallback(int errorCode, const char* description)
{
	Log::error("GLFW error occured {0}: {1}", errorCode, description);
}

void Window::init()
{
	if (!glfwInit())
		ASSERT_MESSAGE(false, "Failed to initialise GLFW");
}

void Window::shutdown()
{
	glfwTerminate();
}

Window::Window(const WindowSpecification& specification)
	: m_specification(specification)
{
	createGLFWWindow();
	setUpOpenGLContext();

	// Set an error callback so it's easy to deal with GLFW errors
	glfwSetErrorCallback(glfwErrorCallback);

	registerGLFWEventCallbacks();

	setVSync(specification.vSyncEnabled);
}

Window::~Window()
{
	glfwDestroyWindow(m_glfwWindow);
}

void Window::onUpdate(TimeStep ts)
{
	glfwSwapBuffers(m_glfwWindow);
	glfwPollEvents();
}

void Window::setVSync(bool vSyncEnabled)
{
	if (vSyncEnabled)
		glfwSwapInterval(1);
	else
		glfwSwapInterval(0);

	m_specification.vSyncEnabled = vSyncEnabled;
}

void Window::createGLFWWindow()
{
	// Set a required minimum version for the OpenGL context

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	m_glfwWindow = glfwCreateWindow(m_specification.width, m_specification.height, m_specification.title.c_str(), NULL, NULL);
	if (!m_glfwWindow)
		ASSERT_MESSAGE(false, "Failed to create GLFW window");

	m_input.m_glfwWindow = m_glfwWindow;

	Log::info("Created GLFW window with title = {0}, width = {1}, height = {2}, VSync enabled = {3}",
		m_specification.title, m_specification.width, m_specification.height, m_specification.vSyncEnabled);
}

void Window::setUpOpenGLContext()
{
	glfwMakeContextCurrent(m_glfwWindow);
	gladLoadGL();
}

void Window::registerGLFWEventCallbacks()
{
	// Set up event callbacks

	// Specify window data that will be available in all of GLFW's callback functions
	glfwSetWindowUserPointer(m_glfwWindow, &m_specification);

	glfwSetWindowCloseCallback(m_glfwWindow, [](GLFWwindow* glfwWindow)
	{
		WindowSpecification* specification = static_cast<WindowSpecification*>(glfwGetWindowUserPointer(glfwWindow));

		if (specification->onWindowCloseCallback)
			specification->onWindowCloseCallback();
	});

	glfwSetWindowSizeCallback(m_glfwWindow, [](GLFWwindow* glfwWindow, int width, int height)
	{
		WindowSpecification* specification = static_cast<WindowSpecification*>(glfwGetWindowUserPointer(glfwWindow));

		specification->width = width;
		specification->height = height;

		if (specification->onWindowResizeCallback)
			specification->onWindowResizeCallback(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
	});

	glfwSetScrollCallback(m_glfwWindow, [](GLFWwindow* glfwWindow, double xOffset, double yOffset)
	{
		WindowSpecification* specification = static_cast<WindowSpecification*>(glfwGetWindowUserPointer(glfwWindow));

		if (specification->onMouseScrollCallback)
			specification->onMouseScrollCallback(static_cast<float>(xOffset), static_cast<float>(yOffset));
	});
}
