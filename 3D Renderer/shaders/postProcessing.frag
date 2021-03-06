#version 450 core

// Input
in vec2 textureCoordinate;

// Output
layout (location = 0) out vec4 fragmentColor;

// Tone mapping functions. https://www.shadertoy.com/view/lslGzl
vec3 LinearToneMapping(vec3 color);
vec3 SimpleReinhardToneMapping(vec3 color);
vec3 LumaBasedReinhardToneMapping(vec3 color);
vec3 WhitePreservingLumaBasedReinhardToneMapping(vec3 color);
vec3 RomBinDaHouseToneMapping(vec3 color);
vec3 FilmicToneMapping(vec3 color);
vec3 Uncharted2ToneMapping(vec3 color);

uniform int toneIndex;
uniform bool blur;

uniform sampler2D multiSampledTexture;
uniform sampler2D gaussianBlurTexture;

void main() {   
  vec3 color = texture(multiSampledTexture, textureCoordinate).rgb;
  if (blur){
    vec3 bloomColor = texture(gaussianBlurTexture, textureCoordinate).rgb;
    color += bloomColor;
  }

  if (toneIndex == 0)
    color = LinearToneMapping(color); 
  else if (toneIndex == 1)
    color = SimpleReinhardToneMapping(color); 
  else if (toneIndex == 2)
    color = LumaBasedReinhardToneMapping(color);
  else if (toneIndex == 3)
    color = WhitePreservingLumaBasedReinhardToneMapping(color); 
  else if (toneIndex == 4)
    color = RomBinDaHouseToneMapping(color); 
  else if (toneIndex == 5)
    color = FilmicToneMapping(color); 
  else if (toneIndex == 6)
    color = Uncharted2ToneMapping(color); 

  fragmentColor = vec4(color, 1.0f);
}

vec3 LinearToneMapping(vec3 color) {
	float exposure = 1.0f;
	color = clamp(exposure * color, 0.0f, 1.0f);
	color = pow(color, vec3(1.0f / 2.2f));
	return color;
}

vec3 SimpleReinhardToneMapping(vec3 color) {
	float exposure = 1.5;
	color *= exposure/(1. + color / exposure);
	color = pow(color, vec3(1. / 2.2f));
	return color;
}

vec3 LumaBasedReinhardToneMapping(vec3 color) {
	float luma = dot(color, vec3(0.2126, 0.7152, 0.0722));
	float toneMappedLuma = luma / (1. + luma);
	color *= toneMappedLuma / luma;
	color = pow(color, vec3(1. / 2.2f));
	return color;
}

vec3 WhitePreservingLumaBasedReinhardToneMapping(vec3 color) {
	float white = 2.;
	float luma = dot(color, vec3(0.2126, 0.7152, 0.0722));
	float toneMappedLuma = luma * (1. + luma / (white*white)) / (1. + luma);
	color *= toneMappedLuma / luma;
	color = pow(color, vec3(1. / 2.2f));
	return color;
}

vec3 RomBinDaHouseToneMapping(vec3 color) {
    color = exp( -1.0 / ( 2.72*color + 0.15 ) );
	color = pow(color, vec3(1. / 2.2f));
	return color;
}

vec3 FilmicToneMapping(vec3 color) {
	color = max(vec3(0.), color - vec3(0.004));
	color = (color * (6.2 * color + .5)) / (color * (6.2 * color + 1.7) + 0.06);
	return color;
}

vec3 Uncharted2ToneMapping(vec3 color) {
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	float W = 11.2;
	float exposure = 2.;
	color *= exposure;
	color = ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
	float white = ((W * (A * W + C * B) + D * E) / (W * (A * W + B) + D * F)) - E / F;
	color /= white;
	color = pow(color, vec3(1.0f / 2.2f));
	return color;
}