#include "PCH.h"
#include "ShaderDataType.h"

GLenum convertShaderDataTypeToOpenGLBaseType(ShaderDataType dataType)
{
	switch (dataType)
	{
	case ShaderDataType::FLOAT:  return GL_FLOAT;        break;
	case ShaderDataType::FLOAT2: return GL_FLOAT;        break;
	case ShaderDataType::FLOAT3: return GL_FLOAT;        break;
	case ShaderDataType::FLOAT4: return GL_FLOAT;        break;
	case ShaderDataType::UINT:   return GL_UNSIGNED_INT; break;
	case ShaderDataType::UINT2:  return GL_UNSIGNED_INT; break;
	case ShaderDataType::UINT3:  return GL_UNSIGNED_INT; break;
	case ShaderDataType::UINT4:  return GL_UNSIGNED_INT; break;
	case ShaderDataType::INT:    return GL_INT;          break;
	case ShaderDataType::INT2:   return GL_INT;          break;
	case ShaderDataType::INT3:   return GL_INT;          break;
	case ShaderDataType::INT4:   return GL_INT;          break;
	case ShaderDataType::MAT2:   return GL_FLOAT;        break;
	case ShaderDataType::MAT3:   return GL_FLOAT;        break;
	case ShaderDataType::MAT4:   return GL_FLOAT;        break;
	default:
		ASSERT_MESSAGE(false, "dataType cannot be converted to OpenGL Base Type");
		return 0;
		break;
	}
}

size_t getShaderDataTypeSize(ShaderDataType dataType)
{
	switch (dataType)
	{
	case ShaderDataType::FLOAT:  return 4;         break;
	case ShaderDataType::FLOAT2: return 4 * 2;     break;
	case ShaderDataType::FLOAT3: return 4 * 3;     break;
	case ShaderDataType::FLOAT4: return 4 * 4;     break;
	case ShaderDataType::UINT:   return 4;         break;
	case ShaderDataType::UINT2:  return 4 * 2;     break;
	case ShaderDataType::UINT3:  return 4 * 3;     break;
	case ShaderDataType::UINT4:  return 4 * 4;     break;
	case ShaderDataType::INT:    return 4;         break;
	case ShaderDataType::INT2:   return 4 * 2;     break;
	case ShaderDataType::INT3:   return 4 * 3;     break;
	case ShaderDataType::INT4:   return 4 * 4;     break;
	case ShaderDataType::MAT2:   return 4 * 2 * 2; break;
	case ShaderDataType::MAT3:   return 4 * 3 * 3; break;
	case ShaderDataType::MAT4:   return 4 * 4 * 4; break;
	default:
		ASSERT_MESSAGE(false, "Cannot get size of dataType");
		return 0;
		break;
	}
}

uint32_t getShaderDataTypeComponentCount(ShaderDataType dataType)
{
	switch (dataType)
	{
	case ShaderDataType::FLOAT:  return 1;     break;
	case ShaderDataType::FLOAT2: return 2;     break;
	case ShaderDataType::FLOAT3: return 3;     break;
	case ShaderDataType::FLOAT4: return 4;     break;
	case ShaderDataType::UINT:   return 1;     break;
	case ShaderDataType::UINT2:  return 2;     break;
	case ShaderDataType::UINT3:  return 3;     break;
	case ShaderDataType::UINT4:  return 4;     break;
	case ShaderDataType::INT:    return 1;     break;
	case ShaderDataType::INT2:   return 2;     break;
	case ShaderDataType::INT3:   return 3;     break;
	case ShaderDataType::INT4:   return 4;     break;
	case ShaderDataType::MAT2:   return 2 * 2; break;
	case ShaderDataType::MAT3:   return 3 * 3; break;
	case ShaderDataType::MAT4:   return 4 * 4; break;
	default:
		ASSERT_MESSAGE(false, "Cannot get the number of components of dataType");
		return 0;
		break;
	}
}
