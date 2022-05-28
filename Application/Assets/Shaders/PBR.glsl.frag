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
    vec4 baseColor;
    float roughness;
    float metalness;
    sampler2D baseColorMap;
    sampler2D roughnessMap;
    sampler2D metalnessMap;
    sampler2D normalMap;

    bool useNormalMap;
};

uniform Material u_material;

// Point lights

struct PointLight
{
    vec3 worldPosition;
    vec3 lightColor;
    float luminousPower;
    float lightRadius;
};

const uint MAX_NUMBER_OF_POINT_LIGHTS = 128;
uniform PointLight u_pointLights[MAX_NUMBER_OF_POINT_LIGHTS];
uniform uint u_pointLightNumber;

uniform vec3 u_viewPosition;

uniform float u_exposure;

// OUTPUTS

layout(location = 0) out vec4 o_fragColor;

// CONSTANTS

const vec3 F0_FOR_DIELECTRICS = vec3(0.04f);
const float PI = 3.14159265359;

// GLOBAL DATA

struct Directions
{
    vec3 normal;
    vec3 viewDirection;
};

Directions g_directions;

struct DotProducts
{
    float nDotV;
};

DotProducts g_dotProducts;

struct MaterialProperties
{
    vec3 baseColor;
    float alpha;
    float roughness;
    float metalness;

    vec3 f0;
};

MaterialProperties g_materialProperties;

// FUNCTIONS

/*
Used the Sclick approximation to calculate the Fresnel reflectance
*/
vec3 calculateFresnelSchlickApproximation(vec3 f0, float u)
{
    return f0 + (1 - f0) * pow((1 - u), 5.0f);
}

/*
Used the Smith height-correlated masking-shadowing function.

As pointed out by Lagarde, using a combination of Smith and the GGX NDF
in the specular (surface reflection) BRDF term means optimisations can be made.

Namely, G(l,v) / (4 * |n.l| * |n.v|) can be simplified. Hammon gives an accurate 
approximation for the above term. This approximation is being calculated in the function.

See https://www.gdcvault.com/play/1024478/PBR-Diffuse-Lighting-for-GGX
*/
float calculateHammonSmithMaskingSpecularDenominatorAppoximation(float nDotL)
{
    float alpha = g_materialProperties.roughness * g_materialProperties.roughness;
    float x = 2.0f * abs(nDotL) * abs(g_dotProducts.nDotV);
    float y = abs(nDotL) + abs(g_dotProducts.nDotV);
    return 1.0f / (2.0f * mix(x, y, alpha));
}

/*
Use the GGX (Trowbridge-Reitz) distribution for the NDF.

Also used the Disney mapping of alpha = roughness * roughness, where roughness
then gives a perceptually linear change from [0, 1].
*/
float calculateGGXDistribution(float nDotH)
{
    float alpha = g_materialProperties.roughness * g_materialProperties.roughness;
    float alpha2 = alpha * alpha;
    float x = 1 + (nDotH * nDotH * (alpha2 - 1.0f));
    return alpha2 / (PI * x * x);
}

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

vec3 calculatePointLightContribution(const PointLight pointLight)
{
    // Calculate the incoming radiance from the point light

    float lightDistance = length(pointLight.worldPosition - vertex_output.worldPosition);
    float lightAttenuationFactor = calculatePointLightAttenuationFactor(lightDistance, pointLight.lightRadius);

    float luminousIntensity = pointLight.luminousPower / (4.0f * PI);
    vec3 lightRadiance = pointLight.lightColor * luminousIntensity * lightAttenuationFactor;

    // Initialise values

    vec3 lightDirection = normalize(pointLight.worldPosition - vertex_output.worldPosition);
    vec3 halfVector = normalize(g_directions.viewDirection + lightDirection);

    float nDotL = dot(g_directions.normal, lightDirection);
    float hDotL = dot(halfVector, lightDirection);
    float nDotH = dot(g_directions.normal, halfVector);

    // Specular (surface reflection) term

    // fresnelReflectance is also the specularTermContribution
    vec3 fresnelReflectance = calculateFresnelSchlickApproximation(g_materialProperties.f0, max(hDotL, 0.0f));
    float hammonSmithMaskingSpecularDenominatorApproximation = calculateHammonSmithMaskingSpecularDenominatorAppoximation(nDotL);
    float NDF = calculateGGXDistribution(nDotH);

    vec3 specularTerm = fresnelReflectance * hammonSmithMaskingSpecularDenominatorApproximation * NDF;

    // Diffuse (sub-surface reflection) term

    vec3 diffuseTermContribution = (vec3(1.0f) - fresnelReflectance) * (1.0f - g_materialProperties.metalness);
    vec3 diffuseTerm = diffuseTermContribution * (g_materialProperties.baseColor / PI);

    // Integrate the reflectance equation with respect to this light

    vec3 BRDFValue = diffuseTerm + specularTerm;
    return BRDFValue * lightRadiance * max(nDotL, 0.0f);
}

void main()
{
    // Initialise global values

    g_directions.normal = getNormalisedSurfaceNormal();
    g_directions.viewDirection = normalize(u_viewPosition - vertex_output.worldPosition);

    g_dotProducts.nDotV = dot(g_directions.normal, g_directions.viewDirection);

    vec4 baseColorWithAlpha = texture(u_material.baseColorMap, vertex_output.textureCoordinates) * u_material.baseColor;
    g_materialProperties.baseColor = baseColorWithAlpha.rgb;
    g_materialProperties.alpha = baseColorWithAlpha.a;
    g_materialProperties.roughness = texture(u_material.roughnessMap, vertex_output.textureCoordinates).r * u_material.roughness;
    g_materialProperties.metalness = texture(u_material.metalnessMap, vertex_output.textureCoordinates).r * u_material.metalness;
    g_materialProperties.f0 = mix(F0_FOR_DIELECTRICS, g_materialProperties.baseColor, g_materialProperties.metalness);

    // Solve the reflectance equation by evaluating the contribution of each point light

    vec3 fragmentColor = vec3(0.0f);

    for (uint i = 0; i < u_pointLightNumber; i++)
        fragmentColor += calculatePointLightContribution(u_pointLights[i]);
    
    // Apply a rudimentary ambient term

    vec3 ambientTerm = vec3(0.05f) * g_materialProperties.baseColor;
    fragmentColor += ambientTerm;

    // Output the shaded color

    o_fragColor = vec4(fragmentColor, g_materialProperties.alpha);
}