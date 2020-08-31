// Learn OpenGL https://learnopengl.com/
// ThinMatrix OpenGL Skeletal Animation Tutorials https://www.youtube.com/watch?v=f3Cr8Yx3GGA&list=PLRIWtICgwaX2tKWCxdeB7Wv_rTET9JtWW

#version 450 core

#define MAX_JOINTS 50
#define MAX_WEIGHTS 3

// Input
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexTextureCoordinate;
layout (location = 3) in vec3 vertexTangent;
layout (location = 4) in vec3 vertexBiTangent;
layout (location = 5) in ivec3 vertexJointIds;
layout (location = 6) in vec3 vertexWeights;
layout (location = 7) in int vertexAlbedoIndex;
layout (location = 8) in int vertexMetalnessIndex;
layout (location = 9) in int vertexNormalIndex;
layout (location = 10) in int vertexRoughnessIndex;
layout (location = 11) in int vertexAmbientOcclusionIndex;

// Output
out VertexOut {
  vec4 position;
  vec3 normal;
  mat3 tangentBiTangentNormalMatrix;
  vec2 textureCoordinate;
  flat int albedoIndex;
  flat int metalnessIndex;
  flat int normalIndex;
  flat int roughnessIndex;
  flat int ambientOcclusionIndex;
} vertexOut;

// Transform
uniform vec3 viewPosition;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat3 normalMatrix; 
uniform mat4 viewProjectionMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform mat4 jointTransforms[MAX_JOINTS];
uniform bool isAnimated;

// Calculate gl_position if animated or static. Pass position and normal. 
// Pass texture coordinate and indicies.
void main() {
  if(isAnimated) {
    mat4 jointTransform = jointTransforms[vertexJointIds[0]] * vertexWeights[0];
    jointTransform += jointTransforms[vertexJointIds[1]] * vertexWeights[1];
    jointTransform += jointTransforms[vertexJointIds[2]] * vertexWeights[2];

    vec4 posePosition = jointTransform * vec4(vertexPosition, 1.0f);
    vertexOut.position = modelMatrix * posePosition;
    gl_Position = viewProjectionMatrix * vertexOut.position;

    vec4 worldNormal = jointTransform * vec4(vertexNormal, 0.0f);
    vertexOut.normal = normalize(normalMatrix * worldNormal.xyz);

    vec4 tangent = jointTransform * vec4(vertexTangent, 0.0f);
    vec4 biTangent = jointTransform * vec4(vertexBiTangent, 0.0f);
    vertexOut.tangentBiTangentNormalMatrix = mat3(tangent, biTangent, vertexOut.normal);
  }
  else {
    vertexOut.position = modelMatrix * vec4(vertexPosition, 1.0f);
    vertexOut.normal = normalize(normalMatrix * vertexNormal);
    vec3 tangent = normalize(normalMatrix * vertexTangent);
    vec3 biTangent = normalize(normalMatrix * vertexBiTangent);
    vertexOut.tangentBiTangentNormalMatrix = mat3(tangent, biTangent, vertexOut.normal);

    gl_Position = modelViewProjectionMatrix * vec4(vertexPosition, 1.0f);
  }

  vertexOut.textureCoordinate = vertexTextureCoordinate;
  vertexOut.albedoIndex = vertexAlbedoIndex;
  vertexOut.metalnessIndex = vertexMetalnessIndex;
  vertexOut.normalIndex = vertexNormalIndex;
  vertexOut.roughnessIndex = vertexRoughnessIndex;
  vertexOut.ambientOcclusionIndex = vertexAmbientOcclusionIndex;
}