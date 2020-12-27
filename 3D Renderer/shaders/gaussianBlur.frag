#version 450 core

// Input
in vec2 textureCoordinate;

// Output
layout (location = 0) out vec4 fragmentColor;

uniform sampler2D pingPongTexture;

uniform bool horizontal;
uniform float weights[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);
uniform int offsets[5] = int[](0, 1, 2, 3, 4);

void main() {             
  ivec2 coordinate = ivec2(gl_FragCoord.xy);
  vec4 sum = texelFetch(pingPongTexture, coordinate, 0) * weights[0];
  
  if(horizontal)
    for(int i = 1; i < 5; ++i) {
      sum += texelFetchOffset(pingPongTexture, coordinate, 0, ivec2(0, offsets[i])) * weights[i];
      sum += texelFetchOffset(pingPongTexture, coordinate, 0, ivec2(0, -offsets[i])) * weights[i];
    }
  else
    for(int i = 1; i < 5; ++i) {
       sum += texelFetchOffset(pingPongTexture, coordinate, 0, ivec2(offsets[i], 0)) * weights[i];
       sum += texelFetchOffset(pingPongTexture, coordinate, 0, ivec2(-offsets[i], 0)) * weights[i];
    }
  
  fragmentColor = sum;
}