#include "PostProcessing.h"

void PostProcessing::Load() {
  LoadQuadrangle();
  LoadMSAA();
  LoadBlitMSAA();
  LoadPingPong();
}

void PostProcessing::Draw(ShaderProgram& shaderProgram) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glActiveTexture(GL_TEXTURE0 + blitMultiSampledTextureLocation);
  glBindTexture(GL_TEXTURE_2D, blitMultiSampledTextureID[0]);
  shaderProgram.SetUniformInt("multiSampledTexture", blitMultiSampledTextureLocation);

  glActiveTexture(GL_TEXTURE0 + pingPongTextureLocation);
  glBindTexture(GL_TEXTURE_2D, pingPongTextureID[0]);
  shaderProgram.SetUniformInt("gaussianBlurTexture", pingPongTextureLocation);

  shaderProgram.SetUniformInt("toneIndex", GLOBAL::WINDOW::TONE_MAPPING_INDEX);
  shaderProgram.SetUniformBool("blur", GLOBAL::WINDOW::BLUR);

  glBindVertexArray(quadrangleVAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}

void PostProcessing::Delete() {}

void PostProcessing::UnbindFrameBuffer() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void PostProcessing::ClearSecondBufferForBloom() {
  glClearTexImage(multiSampledTextureID[1], 0, GL_RGBA, GL_FLOAT, 0);
}

void PostProcessing::BindMultiSampledFramebuffer() {
  glBindFramebuffer(GL_FRAMEBUFFER, multiSampledFboID);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glStencilMask(0x00);
}

void PostProcessing::BlitMultiSampledFramebuffer() {
  glBindFramebuffer(GL_READ_FRAMEBUFFER, multiSampledFboID);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, blitMultiSampledFboID);

  glReadBuffer(GL_COLOR_ATTACHMENT0);
  glDrawBuffer(GL_COLOR_ATTACHMENT0);
  glBlitFramebuffer(0, 0, GLOBAL::WINDOW::WIDTH, GLOBAL::WINDOW::HEIGHT,
    0, 0, GLOBAL::WINDOW::WIDTH, GLOBAL::WINDOW::HEIGHT,
    GL_COLOR_BUFFER_BIT, GL_NEAREST);

  glReadBuffer(GL_COLOR_ATTACHMENT1);
  glDrawBuffer(GL_COLOR_ATTACHMENT1);
  glBlitFramebuffer(0, 0, GLOBAL::WINDOW::WIDTH, GLOBAL::WINDOW::HEIGHT,
    0, 0, GLOBAL::WINDOW::WIDTH, GLOBAL::WINDOW::HEIGHT,
    GL_COLOR_BUFFER_BIT, GL_NEAREST);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcessing::CreateBloomTexture(ShaderProgram& shaderProgram) {
  bool horizontal = true, first_iteration = true;
  int amount = 10;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (unsigned int i = 0; i < amount; i++) {
    glBindFramebuffer(GL_FRAMEBUFFER, pingPongFboID[horizontal]);

    glActiveTexture(GL_TEXTURE0 + pingPongTextureLocation);
    glBindTexture(GL_TEXTURE_2D, first_iteration ? blitMultiSampledTextureID[1] : pingPongTextureID[!horizontal]);
    shaderProgram.SetUniformInt("pingPongTexture", pingPongTextureLocation);
    shaderProgram.SetUniformInt("horizontal", horizontal);

    glBindVertexArray(quadrangleVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    horizontal = !horizontal;
    if (first_iteration)
      first_iteration = false;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

PostProcessing::PostProcessing() {
  quadrangleVAO = 0;
  multiSampledFboID = 0;
  multiSampledTextureID[0] = 0;
  multiSampledTextureID[1] = 0;
  blitMultiSampledFboID = 0;
  blitMultiSampledTextureID[0] = 0;
  blitMultiSampledTextureID[1] = 0;
  pingPongFboID[0] = 0;
  pingPongFboID[1] = 0;
  pingPongTextureID[0] = 0;
  pingPongTextureID[1] = 0;
}

void PostProcessing::LoadQuadrangle() {
  float vertices[] = {
    -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
     1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
     1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
  };

  GLuint vbo;
  glGenVertexArrays(1, &quadrangleVAO);
  glGenBuffers(1, &vbo);
  glBindVertexArray(quadrangleVAO);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
}

void PostProcessing::LoadMSAA() {
  glGenFramebuffers(1, &multiSampledFboID);
  glBindFramebuffer(GL_FRAMEBUFFER, multiSampledFboID);

  glGenTextures(2, multiSampledTextureID);

  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, multiSampledTextureID[0]);
  glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA16F, GLOBAL::WINDOW::WIDTH, GLOBAL::WINDOW::HEIGHT, GL_TRUE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, multiSampledTextureID[0], 0);

  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, multiSampledTextureID[1]);
  glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA16F, GLOBAL::WINDOW::WIDTH, GLOBAL::WINDOW::HEIGHT, GL_TRUE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D_MULTISAMPLE, multiSampledTextureID[1], 0);

  GLuint renderBuffer;
  glGenRenderbuffers(1, &renderBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
  glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, GLOBAL::WINDOW::WIDTH, GLOBAL::WINDOW::HEIGHT);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);

  const GLenum colorAttachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
  glDrawBuffers(2, colorAttachments);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    printf("Framebuffer not complete for multi-sampled\n");

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcessing::LoadBlitMSAA() {
  glGenFramebuffers(1, &blitMultiSampledFboID);
  glBindFramebuffer(GL_FRAMEBUFFER, blitMultiSampledFboID);

  glGenTextures(1, &blitMultiSampledTextureID[0]);
  glBindTexture(GL_TEXTURE_2D, blitMultiSampledTextureID[0]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, GLOBAL::WINDOW::WIDTH, GLOBAL::WINDOW::HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blitMultiSampledTextureID[0], 0);

  glGenTextures(1, &blitMultiSampledTextureID[1]);
  glBindTexture(GL_TEXTURE_2D, blitMultiSampledTextureID[1]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, GLOBAL::WINDOW::WIDTH, GLOBAL::WINDOW::HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, blitMultiSampledTextureID[1], 0);

  GLuint renderBuffer;
  glGenRenderbuffers(1, &renderBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, GLOBAL::WINDOW::WIDTH, GLOBAL::WINDOW::HEIGHT);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    printf("Framebuffer not complete for blit-multi-sampled\n");

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcessing::LoadPingPong() {
  glGenFramebuffers(2, pingPongFboID);
  glGenTextures(2, pingPongTextureID);

  glBindFramebuffer(GL_FRAMEBUFFER, pingPongFboID[0]);
  glBindTexture(GL_TEXTURE_2D, pingPongTextureID[0]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, GLOBAL::WINDOW::WIDTH, GLOBAL::WINDOW::HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingPongTextureID[0], 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    printf("Framebuffer not complete for ping pong\n");

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glBindFramebuffer(GL_FRAMEBUFFER, pingPongFboID[1]);
  glBindTexture(GL_TEXTURE_2D, pingPongTextureID[1]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, GLOBAL::WINDOW::WIDTH, GLOBAL::WINDOW::HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingPongTextureID[1], 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    printf("Framebuffer not complete for ping pong\n");

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
