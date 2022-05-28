#pragma once
#include "PCH.h"

#include "ShaderDataType.h"

class VertexBuffer;

class VertexBufferAttributeSpecification
{
public:

	VertexBufferAttributeSpecification() = delete;
	VertexBufferAttributeSpecification(ShaderDataType dataType, const std::string& name, bool normalise = false)
		: m_dataType(dataType), m_name(name), m_normalise(normalise),
		m_componentCount(getShaderDataTypeComponentCount(dataType)),
		m_size(getShaderDataTypeSize(dataType)) {}

private:
	
	bool isIntType() const;

private:

	ShaderDataType m_dataType;
	std::string m_name;
	bool m_normalise;

	uint32_t m_componentCount;
	size_t m_size;

	friend class VertexBufferLayout;
};

class VertexBufferLayout
{
public:

	VertexBufferLayout(const std::initializer_list<VertexBufferAttributeSpecification>& attributes);

private:

	void bind() const;

private:

	std::vector<VertexBufferAttributeSpecification> m_attributes;
	uint32_t m_stride = 0;

	friend class VertexBuffer;
};