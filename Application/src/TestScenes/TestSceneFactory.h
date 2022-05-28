#pragma once
#include "PCH.h"

#include "TestScene.h"

class TestSceneFactory
{
public:

	enum class TestSceneIdentifier
	{
		FRESNEL_SCENE = 0,
		HDR_SCENE,
		FRAME_TIME_SCENE
	};

public:

	static Reference<TestScene> create(TestSceneIdentifier testSceneIdentifier);
};