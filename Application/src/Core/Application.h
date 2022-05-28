#pragma once
#include "PCH.h"

#include "Platform/Window.h"
#include "Platform/Input.h"
#include "Workspace.h"

class Application
{
public:
	
	static void init();

	static void run();

	static void shutdown();

	static void onWindowCloseEvent();
	static void onWindowResizeEvent(uint32_t width, uint32_t height);
	static void onMouseScrollEvent(float xOffset, float yOffset);

	static const Window& getWindow() { return *s_window; }
	static const Input& getInput() { return s_window->getInput(); }

private:
	
	static bool s_running;
	static Window* s_window;
	static float s_timeAtLastFrame;
	static Workspace* s_workspace;
};