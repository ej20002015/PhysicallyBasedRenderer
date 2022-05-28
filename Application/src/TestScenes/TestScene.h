#pragma once
#include "PCH.h"

#include "Scene/Scene.h"

class TestScene
{
public:

	virtual ~TestScene() = default;

	virtual Reference<BlinnPhongScene> getBlinnPhongScene() const = 0;
	virtual Reference<PBRScene> getPBRScene() const = 0;
};