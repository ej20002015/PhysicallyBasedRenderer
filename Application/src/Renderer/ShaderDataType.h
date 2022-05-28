#pragma once
#include "PCH.h"

#include "glad/glad.h"

enum class ShaderDataType
{
	NONE = 0,
	FLOAT, FLOAT2, FLOAT3, FLOAT4,
	UINT, UINT2, UINT3, UINT4,
	INT, INT2, INT3, INT4,
	MAT2, MAT3, MAT4
};

GLenum convertShaderDataTypeToOpenGLBaseType(ShaderDataType dataType);
size_t getShaderDataTypeSize(ShaderDataType dataType);
uint32_t getShaderDataTypeComponentCount(ShaderDataType dataType);