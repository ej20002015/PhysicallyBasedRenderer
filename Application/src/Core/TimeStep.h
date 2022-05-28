#pragma once
#include "PCH.h"

class TimeStep
{
public:

	TimeStep() = default;
	TimeStep(float time)
		: m_time(time) {}

	// Allow TimeStep to be used as a const float
	operator float() const { return m_time; }

private:

	float m_time = 0.0f;
};