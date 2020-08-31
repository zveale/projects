#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include <string>
#include "glad\glad.h"
#include "sdl\SDL.h"
#include "sdl\SDL_opengl.h"
#include "glm\vec3.hpp"
#include "glm\mat4x4.hpp"
#include "glm\mat3x3.hpp"
#include "ShaderProgram.h"
#include "GameObject.h"
#include "PostProcessing.h"
#include "ReflectionProbe.h"

class RenderSystem {
public:
  RenderSystem();

  /*
  Initialize SDL subsystems, intialize OpenGL loader and set OpenGl state, and load shaders.
  */
  void StartUp();

  void ShutDown();

  /*
  Render loop. Draw dynamic shadow maps, scene, skybox, windows, and lights. Post processing and swap buffers.
  */
  void Render();

  /*
  Setup bindless textures, draw static shadow maps, and load reflection probe.
  */
  void LateStartUp();

  void AttachGameObjects(GameObject** gameobject);

private:
  struct AllTextures {
    GLuint64 textures[100];
  } allTextures;
  GLuint UBO;

  ReflectionProbe reflectionProbe;
  PostProcessing postProcessing;
  SceneManager sceneManager;
  GameObject** gameObjects;

  ShaderProgram lampProgram, skyboxProgram, directionalShadowProgram, refractionProgram, blendProgram, 
    postProcessingProgram, pbrProgram, equirectangualrToCubeMapProgram, convolutionProgram, prefilterProgram, 
    bdrfProgram, reflectionProbeProgram, queryProgram, gaussianBlurProgram;
  void LoadShaders();

  SDL_Window* window;
  SDL_GLContext context;
  void CreateSDLWindowAndContext();
  void InitializeWindow();
  void InitializeGLADandOpenGL();

  const float aspect;
  const float nearPlane;
  const float farPlane;
  float* fov;

  glm::mat4* viewMatrix;
  glm::mat4 projectionMatrix;
  glm::mat4 viewportMatrix;

  void ResizeViewport();
  void SetViewMatrixAndFOV();
  void CheckAndClearViewport();
  void UpdateViewProjectionMatrix();
};

#endif // !RENDERERSYSTEM_H

/*
texture locations :  name
49  :  skyBox
48  :  cubemap
47  :  irradiance
46  :  prefilter 
45  :  bdrf
44  :
43  :  blur
42  :  msaa
41  :  jitter
40  :  shadows
39  :  shadows
38  :  shadows
37  :  shadows
36  :  shadows
*/

