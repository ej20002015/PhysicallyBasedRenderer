#version 460 core

// ATTRIBUTES

layout (location = 0) in vec2 a_position;
layout (location = 1) in vec2 a_textureCoordinates;

// OUTPUTS

struct VertexOutput
{
    vec2 textureCoordinates;
};

out VertexOutput vertex_output;

// FUNCTIONS

void main()
{
    vertex_output.textureCoordinates = a_textureCoordinates;
    gl_Position = vec4(a_position, 0.0f, 1.0f);
}