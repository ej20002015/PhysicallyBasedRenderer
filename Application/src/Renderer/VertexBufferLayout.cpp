#include "PCH.h"
#include "VertexBufferLayout.h"

VertexBufferLayout::VertexBufferLayout(const std::initializer_list<VertexBufferAttributeSpecification>& attributes)
	: m_attributes(attributes)
{
	m_stride = static_cast<uint32_t>(std::accumulate(m_attributes.begin(), m_attributes.end(), 0, [](int i, const auto& attribute)
	{
		return static_cast<uint32_t>(attribute.m_size + i);
	}));
}

void VertexBufferLayout::bind() const
{
	uint32_t currentOffset = 0;
	uint32_t currentIndex = 0;

	for (VertexBufferAttributeSpecification attribute : m_attributes)
	{
		GLenum openGLBaseType = convertShaderDataTypeToOpenGLBaseType(attribute.m_dataType);

		if (attribute.isIntType())
		{
			glVertexAttribIPointer
			(
				currentIndex,
				attribute.m_componentCount,
				openGLBaseType,
				m_stride,
				reinterpret_cast<void*>(static_cast<uint64_t>((currentOffset)))
			);
		}
		else
		{
			glVertexAttribPointer
			(
				currentIndex,
				attribute.m_componentCount,
				openGLBaseType,
				attribute.m_normalise,
				m_stride,
				reinterpret_cast<void*>(static_cast<uint64_t>((currentOffset)))
			);
		}

		glEnableVertexAttribArray(currentIndex);

		currentIndex++;
		currentOffset += static_cast<uint32_t>(attribute.m_size);
	}

	Log::trace("Specified Vertex Buffer Layout to OpenGL");
}

bool VertexBufferAttributeSpecification::isIntType() const
{
	switch (m_dataType)
	{
	case ShaderDataType::FLOAT:  return false; break;
	case ShaderDataType::FLOAT2: return false; break;
	case ShaderDataType::FLOAT3: return false; break;
	case ShaderDataType::FLOAT4: return false; break;
	case ShaderDataType::UINT:	 return true;  break;
	case ShaderDataType::UINT2:	 return true;  break;
	case ShaderDataType::UINT3:	 return true;  break;
	case ShaderDataType::UINT4:	 return true;  break;
	case ShaderDataType::INT:	 return true;  break;
	case ShaderDataType::INT2:	 return true;  break;
	case ShaderDataType::INT3:	 return true;  break;
	case ShaderDataType::INT4:	 return true;  break;
	case ShaderDataType::MAT2:	 return false; break;
	case ShaderDataType::MAT3:	 return false; break;
	case ShaderDataType::MAT4:	 return false; break;
	default:
		ASSERT_MESSAGE(false, "Cannot determine if m_dataType is an integer type");
		return 0;
		break;
	}
}
