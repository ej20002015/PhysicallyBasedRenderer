#pragma once
#include "PCH.h"

#include <memory>

#ifdef PBR_DEBUG

	#if PBR_WINDOWS
		#define DEBUG_BREAK __debugbreak();
	#else
		#define DEBUG_BREAK
	#endif

	#define ASSERT(cond) if ((!cond)) { { __debugbreak(); } }
	#define ASSERT_MESSAGE(cond, ...) { if (!(cond)) { Log::critical(std::string("Assertion failed: ") + __VA_ARGS__); DEBUG_BREAK } }

#else

	#define ASSERT(cond) {}
	#define ASSERT_MESSAGE(cond, ...) {}

#endif

static constexpr uint32_t DEFAULT_WIDTH = 1280;
static constexpr uint32_t DEFAULT_HEIGHT = 720;
static constexpr float DEFAULT_ASPECT_RATIO = static_cast<float>(DEFAULT_WIDTH) / static_cast<float>(DEFAULT_HEIGHT);

template<typename T>
using Reference = std::shared_ptr<T>;

template<typename T, typename... Args>
constexpr Reference<T> createReference(Args&&... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T>
using Unique = std::unique_ptr<T>;

template<typename T, typename... Args>
constexpr Unique<T> createUnique(Args&&... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}