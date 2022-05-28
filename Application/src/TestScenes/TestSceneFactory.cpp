#include "PCH.h"
#include "TestSceneFactory.h"

#include "FresnelTestScene.h"
#include "HDRTestScene.h"
#include "FrameTimeTestScene.h"

Reference<TestScene> TestSceneFactory::create(TestSceneIdentifier testSceneIdentifier)
{
	switch (testSceneIdentifier)
	{
	case TestSceneIdentifier::FRESNEL_SCENE:
		return createReference<FresnelTestScene>();
		break;
	case TestSceneIdentifier::HDR_SCENE:
		return createReference<HDRTestScene>();
		break;
	case TestSceneIdentifier::FRAME_TIME_SCENE:
		return createReference<FrameTimeTestScene>();
		break;
	default:
		ASSERT_MESSAGE(false, "Unknown test scene identifier");
		return nullptr;
		break;
	}
}
