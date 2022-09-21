#pragma once
#include "PCH.h"

#include "glfw3.h"

#include "Input.h"

class Window
{
public:

	struct WindowSpecification
	{
		std::string title = "Hello World";
		uint32_t width = DEFAULT_WIDTH, height = DEFAULT_HEIGHT;
		bool vSyncEnabled = false;

		// Redefinition of default values through the default constructor
		// is necessary to get around a gcc bug.
		// See https://bugs.llvm.org/show_bug.cgi?id=36684
		WindowSpecification()
			: title("Hello World"),
			  width(DEFAULT_WIDTH), height(DEFAULT_HEIGHT),
			  vSyncEnabled(false) {}
		
		// Callback event methods

		std::function<void()> onWindowCloseCallback;
		std::function<void(uint32_t, uint32_t)> onWindowResizeCallback;
		std::function<void(float, float)> onMouseScrollCallback;
	};

public:

	// Initialise the windowing system
	static void init();

	// Shutdown the windowing system
	static void shutdown();

	Window(const WindowSpecification& specification = WindowSpecification());
	~Window();

	void onUpdate(TimeStep ts);

	float getWindowTime() const { return static_cast<float>(glfwGetTime()); }

	const Input& getInput() const { return m_input; }

	bool getVSyncEnabled() const { return m_specification.vSyncEnabled; }
	void setVSync(bool vSyncEnabled);

	uint32_t getWidth() const { return m_specification.width; }
	uint32_t getHeight() const { return m_specification.height; }

private:

	void createGLFWWindow();
	void setUpOpenGLContext();
	void registerGLFWEventCallbacks();

private:

	WindowSpecification m_specification;
	GLFWwindow* m_glfwWindow;
	Input m_input;
};