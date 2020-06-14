// Wire frame. OpenGL 4 Cookbook 3rd Edtion. Chapter 7: Using Geometry and Tessellation Shaders

#version 450 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

// Input
in VertexOut {
  vec4 position;
  vec3 normal;
  mat3 tangentBiTangentNormalMatrix;
  vec2 textureCoordinate;
  flat int albedoIndex;
  flat int metalnessIndex;
  flat int normalIndex;
  flat int roughnessIndex;
  flat int ambientOcclusionIndex;
} vertexIn[];

// Output
out GeometryOut {
  vec4 position;
  vec3 normal;
  mat3 tangentBiTangentNormalMatrix;
  vec2 textureCoordinate;
  flat int albedoIndex;
  flat int metalnessIndex;
  flat int normalIndex;
  flat int roughnessIndex;
  flat int ambientOcclusionIndex;
} geometryOut;

noperspective out vec3 edgeDistance;

uniform mat4 viewportMatrix; 

void main() {
  // Transform each vertex into viewport space
  vec3 p0 = vec3(viewportMatrix * (gl_in[0].gl_Position / gl_in[0].gl_Position.w));
  vec3 p1 = vec3(viewportMatrix * (gl_in[1].gl_Position / gl_in[1].gl_Position.w));
  vec3 p2 = vec3(viewportMatrix * (gl_in[2].gl_Position / gl_in[2].gl_Position.w));
  
  // Find the altitudes (ha, hb and hc)
  float a = length(p1 - p2);
  float b = length(p2 - p0);
  float c = length(p1 - p0);
  float alpha = acos((b * b + c * c - a * a) / (2.0f * b *c));
  float beta = acos((a * a + c * c - b * b) / (2.0f * a * c));
  float ha = abs(c * sin(beta));
  float hb = abs(c * sin(alpha));
  float hc = abs(b * sin(alpha));
  
  // Send the triangle along with the edge distances
  for(int i = 0; i < 3; ++i)  {
    if (i == 0)
      edgeDistance = vec3(ha, 0, 0);
    if (i == 1)
      edgeDistance = vec3(0, hb, 0);
    if (i == 2)
      edgeDistance = vec3(0, 0, hc);
 
    geometryOut.position = vertexIn[i].position;
    geometryOut.normal = vertexIn[i].normal;
    geometryOut.tangentBiTangentNormalMatrix = vertexIn[i].tangentBiTangentNormalMatrix;
    geometryOut.textureCoordinate = vertexIn[i].textureCoordinate;
    geometryOut.albedoIndex = vertexIn[i].albedoIndex;
    geometryOut.metalnessIndex = vertexIn[i].metalnessIndex;
    geometryOut.normalIndex = vertexIn[i].normalIndex;
    geometryOut.roughnessIndex = vertexIn[i].roughnessIndex;
    geometryOut.ambientOcclusionIndex = vertexIn[i].ambientOcclusionIndex;
    gl_Position = gl_in[i].gl_Position;

    EmitVertex();
  }    
  
  EndPrimitive();
}