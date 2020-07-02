// Advance OpenGL : Bloom: https://learnopengl.com/Advanced-Lighting/Bloom
// Creating a bloom effect.OpenGL 4 Cookbook 3rd Edition. Chapter 6 : Image Processing and Screen Space Techniques

#ifndef POSTPROCESSING_H
#define POSTPROCESSING_H

#include <cstdio>
#include "glad\glad.h"
#include "ShaderProgram.h"

class PostProcessing {
public:
  PostProcessing();
  void Load();
  void Draw(ShaderProgram& shaderProgram);
  void Delete();
  void UnbindFrameBuffer();
  void ClearSecondBufferForBloom();
  void BindMultiSampledFramebuffer();
  void BlitMultiSampledFramebuffer();
  void CreateBloomTexture(ShaderProgram& shaderProgram);

private:
  GLuint quadrangleVAO;
  void LoadQuadrangle();

  GLuint multiSampledFboID;
  GLuint multiSampledTextureID[2];
  void LoadMSAA();

  GLuint blitMultiSampledFboID;
  GLuint blitMultiSampledTextureID[2];
  const GLuint blitMultiSampledTextureLocation = 42;
  void LoadBlitMSAA();

  GLuint pingPongFboID[2];
  GLuint pingPongTextureID[2];
  const GLuint pingPongTextureLocation = 43;
  void LoadPingPong();
};

#endif // !POSTPROCESSING_H
