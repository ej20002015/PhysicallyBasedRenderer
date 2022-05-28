#version 460 core

// ATTRIBUTES

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_textureCoordinates;
layout (location = 3) in vec3 a_tangent;
layout (location = 4) in vec3 a_bitangent;

// UNIFORMS

uniform mat4 u_transform;
uniform mat4 u_projectionViewMatrix;

// OUTPUTS

struct VertexOutput
{
    vec3 worldPosition;
    vec3 normal;
    vec2 textureCoordinates;
    mat3 TBN;
};

out VertexOutput vertex_output;

// FUNCTIONS

void main()
{
    vertex_output.worldPosition = vec3(u_transform * vec4(a_position, 1.0f));
    vertex_output.normal = normalize(vec3(transpose(inverse(u_transform)) * vec4(a_normal, 0.0f)));
    vertex_output.textureCoordinates = a_textureCoordinates;

    vec3 normalTransformed = normalize(vec3(u_transform * vec4(a_normal, 0.0f)));
    vec3 tangentTransformed = normalize(vec3(u_transform * vec4(a_tangent, 0.0f)));
    vec3 bitangentTransformed = normalize(vec3(u_transform * vec4(a_bitangent, 0.0f)));
    vertex_output.TBN = mat3(tangentTransformed, bitangentTransformed, normalTransformed);

    gl_Position = u_projectionViewMatrix * u_transform * vec4(a_position, 1.0f);
}