#version 460 core

// INPUTS FROM VERTEX SHADER

struct VertexOutput
{
    vec3 worldPosition;
    vec3 normal;
    vec2 textureCoordinates;
    mat3 TBN;
};

in VertexOutput vertex_output;

// UNIFORMS

// Material

struct Material
{
    vec4 diffuseColor;
    vec3 specularColor;
    sampler2D diffuseMap;
    sampler2D specularMap;
    sampler2D normalMap;
    float shininess;

    bool useNormalMap;
};

uniform Material u_material;

// Point lights

struct PointLight
{
    vec3 worldPosition;
    vec3 diffuseComponent;
    vec3 specularComponent;
    float lightRadius;
};

const uint MAX_NUMBER_OF_POINT_LIGHTS = 128;
uniform PointLight u_pointLights[MAX_NUMBER_OF_POINT_LIGHTS];
uniform uint u_pointLightNumber;

uniform vec3 u_viewPosition;

// OUTPUTS

layout(location = 0) out vec4 o_fragColor;

// CONSTANTS

const vec3 LIGHT_AMBIENT = vec3(0.02f);

// GLOBAL DATA

vec3 g_normal;
vec3 g_viewDirection;
vec3 g_diffuseMaterialValue;
vec3 g_specularMaterialValue;

// FUNCTIONS

vec3 getNormalisedSurfaceNormal()
{
    if (u_material.useNormalMap)
    {
        vec4 sampleFromNormalMap = texture(u_material.normalMap, vertex_output.textureCoordinates);
        vec3 sampledNormal = (sampleFromNormalMap.rgb * 2.0f) - 1.0f;
        vec3 sampledNormalInWorldSpace = vertex_output.TBN * sampledNormal;
        return normalize(sampledNormalInWorldSpace);
    }
    else
        return normalize(vertex_output.normal);
}

float calculatePointLightAttenuationFactor(float lightDistance, float lightRadius)
{
    const float lightSize = 0.01f;
    const uint n = 4;

    // Restrict the minimum value of the denominator to 0.01 * 0.01 to avoid the value
    // exploding or having divide by zero errors
    float inverseSquaredDistance = 1.0f / pow(max(lightDistance, lightSize), 2.0f);

    // Use a windowing function to cutoff the attenuation value to 0 at large distances
    
    float lightDistanceNOverLightRadiusN = 1.0f - pow(lightDistance / lightRadius, n);
    float windowingFunctionValue = pow(clamp(lightDistanceNOverLightRadiusN, 0.0f, 1.0f), 2.0f);

    return min(inverseSquaredDistance * windowingFunctionValue, 1.0f);
}

vec3 calculateDiffuseContribution(vec3 lightDirection, vec3 attenuatedLightDiffuseComponent)
{
    float lightIncidentDiffuseFactor = max(dot(lightDirection, g_normal), 0.0f);
    return (g_diffuseMaterialValue * lightIncidentDiffuseFactor) * attenuatedLightDiffuseComponent;
}

vec3 calculateSpecularContribution(vec3 lightDirection, vec3 attenuatedLightSpecularComponent)
{
    vec3 halfVector = normalize(g_viewDirection + lightDirection);
    float lightReflectedSpecularFactor = pow(max(dot(halfVector, g_normal), 0.0f), u_material.shininess);
    return (g_specularMaterialValue * lightReflectedSpecularFactor) * attenuatedLightSpecularComponent;
}

vec3 gammaCorrectColor(vec3 color)
{
    vec3 SRGBEncodedHigher = (1.055f * pow(color, vec3(1.0f / 2.4f))) - 0.055f;
    vec3 SRGBEncodedLower = 12.92f * color;
    float rSRGBEncoded = (color.r > 0.0031308f) ? SRGBEncodedHigher.r : SRGBEncodedLower.r;
    float gSRGBEncoded = (color.g > 0.0031308f) ? SRGBEncodedHigher.g : SRGBEncodedLower.g;
    float bSRGBEncoded = (color.b > 0.0031308f) ? SRGBEncodedHigher.b : SRGBEncodedLower.b;
    return vec3(rSRGBEncoded, gSRGBEncoded, bSRGBEncoded);
}

void main()
{
    vec3 color;
    g_normal = getNormalisedSurfaceNormal();
    g_viewDirection = normalize(u_viewPosition - vertex_output.worldPosition);

    vec4 diffuseMaterialValueWithAlpha = texture(u_material.diffuseMap, vertex_output.textureCoordinates) * u_material.diffuseColor;
    float alpha = diffuseMaterialValueWithAlpha.a;
    
    g_diffuseMaterialValue = diffuseMaterialValueWithAlpha.rgb;
    g_specularMaterialValue = texture(u_material.specularMap, vertex_output.textureCoordinates).rgb * u_material.specularColor;

    // Calculate ambient contribution

    color = g_diffuseMaterialValue * LIGHT_AMBIENT;

    // Calculate diffuse and specular contribution for each light
    
    for (uint i = 0; i < u_pointLightNumber; i++)
    {
        PointLight pointLight = u_pointLights[i];
        vec3 lightDirection = normalize(pointLight.worldPosition - vertex_output.worldPosition);

        float lightDistance = length(pointLight.worldPosition - vertex_output.worldPosition);
        float attenuationFactor = calculatePointLightAttenuationFactor(lightDistance, pointLight.lightRadius);

        color += calculateDiffuseContribution(lightDirection, pointLight.diffuseComponent * attenuationFactor);
        color += calculateSpecularContribution(lightDirection, pointLight.specularComponent * attenuationFactor);
    }
    
    o_fragColor = vec4(gammaCorrectColor(color), alpha);
}