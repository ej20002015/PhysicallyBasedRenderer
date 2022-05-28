#pragma once
#include "PCH.h"

#include "TestScene.h"

class FrameTimeTestScene : public TestScene
{
public:

	FrameTimeTestScene();

	Reference<BlinnPhongScene> getBlinnPhongScene() const { return m_blinnPhongScene; }
	Reference<PBRScene> getPBRScene() const { return m_PBRScene; }

private:

	Reference<BlinnPhongScene> m_blinnPhongScene;
	Reference<PBRScene> m_PBRScene;
};