// Learn OpenGL https://learnopengl.com/PBR/IBL/Specular-IBL
// OpenGL 4 Cookbook 3rd Edition.
// Chetan Jags: Image Based Lighting https://chetanjags.wordpress.com/2015/08/26/image-based-lighting/
// jMonkeyEngine https://wiki.jmonkeyengine.org/jme3/advanced/pbr_part1.html

#version 450 core
#extension GL_ARB_bindless_texture : require

#define MAX_SHADOWS 5
#define MAX_LIGHTS 10

// Input
in GeometryOut {
  vec4 position;
  vec3 normal;
  mat3 tangentBiTangentNormalMatrix;
  vec2 textureCoordinate;
  flat int albedoIndex;
  flat int metalnessIndex;
  flat int normalIndex;
  flat int roughnessIndex;
  flat int ambientOcclusionIndex;
} fragmentIn;

// Output
layout (location = 0) out vec4 fragmentColor;

// Wire frame. OpenGL 4 Cookbook 3rd Edition. Chapter 7: Using Geometry and Tessellation Shaders
noperspective in vec3 edgeDistance;
uniform struct LineInfo {
  float width;
  vec4 color;
} line;

// Light
struct Light {
  int type;
  vec3 position;
  vec3 direction;
  vec3 color;
  float cutoff;
  float outerCutoff;
  float intensity;
  
  int shadowID;
  bool isShadowCasting;
};
uniform int numLights;
uniform Light lights[MAX_LIGHTS];

// Shadow
struct Shadow {
  mat4 matrix;
  float biasFactor;
  float biasMax;
  sampler2DShadow map;
};
uniform int numShadows;
uniform Shadow shadows[MAX_SHADOWS];
uniform sampler3D jitterTexture;
uniform vec3 jitterTextureSize; 
uniform float jitterRadius;

// ARB_BindlessTexture
layout (binding = 0) uniform Block {
    sampler2D sampler2ds[100];
};

uniform vec3 viewPosition;

// IBL
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;
const float PI = 3.14159265359;

// BDRF 
vec3 CookTorranceBRDF(vec3 F, float G, float D, vec3 N, vec3 L, vec3 V);
float DistributionGGX(float NdotH, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 FresnelSchlick(float HdotV, vec3 F0);
vec3 FresnelSchlickRoughness(float HdotV, vec3 F0, float roughness);

vec3 CalculateNormalMap();
vec3 CalculateLightDirection(Light light);
float CalculateLightAttenuation(Light light, vec3 L);

// Chapter 17. Efficient Soft-Edged Shadows Using Pixel Shader Branching: https://developer.nvidia.com/gpugems/gpugems2/part-ii-shading-lighting-and-shadows/chapter-17-efficient-soft-edged-shadows-using
// Soft shadows with random sampling. OpenGL 4 Cookbook 3rd Edition. Chapter 8: Shadows
float CalculateDirectionalShadow(Light light, Shadow shadow, int index);

void main() {		
  vec3 albedo = texture(sampler2ds[fragmentIn.albedoIndex], fragmentIn.textureCoordinate).rgb;
  float metallic = texture(sampler2ds[fragmentIn.metalnessIndex], fragmentIn.textureCoordinate).r;
  vec3 normal = CalculateNormalMap();
  float roughness = texture(sampler2ds[fragmentIn.roughnessIndex], fragmentIn.textureCoordinate).r;
  float ao = texture(sampler2ds[fragmentIn.ambientOcclusionIndex], fragmentIn.textureCoordinate).r;
  
  roughness = clamp(roughness, 0.85f, 0.99f);

  vec3 N = normal;
  vec3 V = normalize(viewPosition - fragmentIn.position.xyz);
  
  vec3 F0 = albedo;
  F0 = mix(F0, albedo, metallic);
  
  vec3 Lo = vec3(0.0);
  int shadowIndex = 0;

  for (int i = 0; i < numLights; ++i) {
    vec3 L = CalculateLightDirection(lights[i]);
    
    vec3 H = normalize(V + L);
    
    vec3 F = FresnelSchlick(max(dot(H, V), 0.0f), F0);
    float G = GeometrySmith(N, V, L, roughness);
    float D = DistributionGGX(max(dot(N, H), 0.0f), roughness);   
    vec3 specular = CookTorranceBRDF(F, G, D, N, L, V);
   
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;	  
    
    float NdotL = max(dot(N, L), 0.0);   

    float attenuation = CalculateLightAttenuation(lights[i], L);
    vec3 radiance = lights[i].color * attenuation;
    
    Lo += (kD * albedo / PI + specular) * radiance * NdotL;

    float shadow  = CalculateDirectionalShadow(lights[i], shadows[shadowIndex], shadowIndex);
    Lo *= shadow;
  }   
  
  vec3 F = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
  
  vec3 kS = F;
  vec3 kD = 1.0 - kS;
  kD *= 1.0 - metallic;	  
  
  vec3 irradiance = texture(irradianceMap, N).rgb;
  vec3 diffuse = irradiance * albedo;

  const float MAX_REFLECTION_LOD = 4.0;
  vec3 R = reflect(-V, N);
  vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;    
  vec2 brdf = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
  vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

  vec3 ambient = (kD * diffuse + specular) * ao;
  vec3 color = ambient + Lo;

  fragmentColor = vec4(color, 1.0);

  // Wire frame. OpenGL 4 Cookbook 3rd Edition. Chapter 7: Using Geometry and Tessellation Shaders
  //float d = min(edgeDistance.x, edgeDistance.y);
  //d = min(d, edgeDistance.z);
  //float mixValue = smoothstep(line.width - 1, line.width + 1, d);
  //fragmentColor = mix(line.color, vec4(color, 1.0), mixValue);
}

vec3 CookTorranceBRDF(vec3 F, float G, float D, vec3 N, vec3 L, vec3 V) {
	return F * G * D / (4 * max(dot(N, L), 0.0f) * max(dot(N, V), 0.0f) + 0.001f);
}

float DistributionGGX(float NdotH, float roughness) {
    float alpha2  = roughness * roughness * roughness * roughness;
    float result = (NdotH * NdotH) * (alpha2 - 1.0f) + 1.0f;

    return alpha2 / (PI * result * result);
}

float GeometrySchlickGGX(float dotProduct, float roughness) {
    float k = (roughness + 1.0f) * (roughness + 1.0f) / 8.0f;
    return dotProduct / (dotProduct * (1.0f - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float ggx2 = GeometrySchlickGGX(max(dot(N, V), 0.0f), roughness);
    float ggx1 = GeometrySchlickGGX(max(dot(N, L), 0.0f), roughness);

    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0f - F0) * pow(1.0f - cosTheta, 5.0f);
}

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0f - roughness), F0) - F0) * pow(1.0f - cosTheta, 5.0f);
}   

vec3 CalculateLightDirection(Light light) {
  if (light.type == 0)
    return normalize(light.direction); 

    return normalize(light.position - fragmentIn.position.xyz);
}

float CalculateLightAttenuation(Light light, vec3 L) {
  float attenuation = light.intensity;
  if (light.type == 2 && !light.isShadowCasting) {
    float theta = dot(L, normalize(-light.direction)); 
    float epsilon = light.cutoff - light.outerCutoff;
    attenuation = clamp((theta - light.outerCutoff) / epsilon, 0.1f, 1.0f);
  }

  float distanceFromLight = max(length(light.position - fragmentIn.position.xyz), 0.001f);
  attenuation /= distanceFromLight * distanceFromLight;
  
  return attenuation;
}

vec3 CalculateNormalMap() {
    vec3 tangentNormal = texture(sampler2ds[fragmentIn.normalIndex], fragmentIn.textureCoordinate).xyz * 2.0 - 1.0;
    return normalize(fragmentIn.tangentBiTangentNormalMatrix * tangentNormal);
}

float CalculateDirectionalShadow(Light light, Shadow shadow, int index) {
  float shadowColor = 1.0f;

  if (light.isShadowCasting) {
    float bias = max(shadow.biasFactor * (1.0f - dot(normalize(fragmentIn.normal), light.direction)), shadow.biasMax);
    
    ivec3 offsetCoordinate = ivec3(mod(gl_FragCoord.xy, jitterTextureSize.xy), 0);
    vec4 shadowCoordinateIn = shadows[index].matrix * fragmentIn.position;
    vec4 shadowCoordinate = shadowCoordinateIn;
    float sum = 0.0;
    
    for(int i = 0; i < 4; i++) {
      offsetCoordinate.z = i;
      vec4 offsets = texelFetch(jitterTexture, offsetCoordinate, 0) * jitterRadius * shadowCoordinateIn.w;
    
      shadowCoordinate.xy = shadowCoordinateIn.xy + offsets.xy;
      sum += textureProj(shadow.map, vec4(shadowCoordinate.xy, shadowCoordinate.z - bias, shadowCoordinate.w), 0.0);
      shadowCoordinate.xy = shadowCoordinateIn.xy + offsets.zw;
      sum += textureProj(shadow.map, vec4(shadowCoordinate.xy, shadowCoordinate.z - bias, shadowCoordinate.w), 0.0);
    }
    
    shadowColor = sum / 8.0f;
    int samplesDiv2 = int(jitterTextureSize.z);
    
    if(shadowColor != 1.0f && shadowColor != 0.0f) {
      for(int i = 4; i < samplesDiv2; i++) {
        offsetCoordinate.z = i;
        vec4 offsets = texelFetch(jitterTexture, offsetCoordinate, 0) * jitterRadius * 
          shadowCoordinateIn.w;
    
        shadowCoordinate.xy = shadowCoordinateIn.xy + offsets.xy;
        sum += textureProj(shadow.map, vec4(shadowCoordinate.xy, shadowCoordinate.z - bias, shadowCoordinate.w), 0.0);
        shadowCoordinate.xy = shadowCoordinateIn.xy + offsets.zw;
        sum += textureProj(shadow.map, vec4(shadowCoordinate.xy, shadowCoordinate.z - bias, shadowCoordinate.w), 0.0);
      }
    
      shadowColor = sum / float(samplesDiv2 * 2.0f);
    }

    ++index;
  }

  return shadowColor;
}