#version 450 core
#extension GL_ARB_bindless_texture : require

#define MAX_SHADOWS 5
#define MAX_LIGHTS 10

// Input
in VertexOut {
  vec4 position;
  vec3 normal;
  vec2 textureCoordinate;
  flat int albedoIndex;
} fragmentIn;

// Output
layout (location = 0) out vec4 fragmentColor;

// ARB_BindlessTexture
layout (binding = 0) uniform Block {
    sampler2D sampler2ds[100];
};

uniform vec3 viewPosition;
uniform samplerCube skybox;
uniform mat4 skyboxModelMatrix;

void main() {		
  vec4 ambient = 0.001f * texture(sampler2ds[fragmentIn.albedoIndex], fragmentIn.textureCoordinate);
  
  vec4 color = ambient;
  
  vec3 directionToVertex = normalize(fragmentIn.position.xyz - viewPosition);
  vec3 refractDirection = refract((skyboxModelMatrix * vec4(directionToVertex, 0.0f)).xyz, normalize(fragmentIn.normal), 0.6f);
  
  vec4 skyboxRefractColor = texture(skybox, refractDirection);
  
  fragmentColor = mix(color, skyboxRefractColor, 0.2f);
}