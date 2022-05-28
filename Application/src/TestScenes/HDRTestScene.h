#pragma once
#include "PCH.h"

#include "TestScene.h"

class HDRTestScene : public TestScene
{
public:

	HDRTestScene();

	Reference<BlinnPhongScene> getBlinnPhongScene() const { return m_blinnPhongScene; }
	Reference<PBRScene> getPBRScene() const { return m_PBRScene; }

private:

	Reference<BlinnPhongScene> m_blinnPhongScene;
	Reference<PBRScene> m_PBRScene;
};
