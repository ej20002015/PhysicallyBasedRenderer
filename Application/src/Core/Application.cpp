#include "PCH.h"
#include "Application.h"

#include "Renderer/Renderer.h"

bool Application::s_running = true;
Window* Application::s_window = nullptr;
float Application::s_timeAtLastFrame = 0.0f;
Workspace* Application::s_workspace = nullptr;

void Application::init()
{
	Log::init();
    Window::init();

    Window::WindowSpecification windowSpecification;
    windowSpecification.title = "Renderer";
    windowSpecification.onWindowCloseCallback = std::bind(Application::onWindowCloseEvent);
    windowSpecification.onWindowResizeCallback = std::bind(Application::onWindowResizeEvent, std::placeholders::_1, std::placeholders::_2);
    windowSpecification.onMouseScrollCallback = std::bind(Application::onMouseScrollEvent, std::placeholders::_1, std::placeholders::_2);

    s_window = new Window(windowSpecification);

    // Init renderer after the OpenGL context has been created
    Renderer::init();

    s_workspace = new Workspace();
}

void Application::run()
{
    while (s_running)
    {
        // Calculate the TimeStep (time between frames)

        float currentTime = s_window->getWindowTime();
        TimeStep ts = currentTime - s_timeAtLastFrame;
        s_timeAtLastFrame = currentTime;

        Log::trace("{0} FPS", 1.0f / ts);

        // Clear default framebuffer (what is presented to the user by GLFW) and then render new frame
           
        Renderer::bindDefaultFramebuffer();
        Renderer::clear();

        s_workspace->onUpdate(ts);

        // Update the window which presents the new frame to the user and processes any events
        s_window->onUpdate(ts);
    }
}

void Application::shutdown()
{
    // Need to call the workspace destructor before shutting down the window (and rendering context)
    delete s_workspace;

    Renderer::shutdown();

    // Need to call the window destructor before shutting down the whole windowing system
    delete s_window;

    Window::shutdown();
}

void Application::onWindowCloseEvent()
{
    s_running = false;
}

void Application::onWindowResizeEvent(uint32_t width, uint32_t height)
{
    Renderer::onWindowResizeEvent(width, height);
    s_workspace->onWindowResizeEvent(width, height);
}

void Application::onMouseScrollEvent(float xOffset, float yOffset)
{
    s_workspace->onMouseScrollEvent(xOffset, yOffset);
}
