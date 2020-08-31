#include <cstdio>
#include <cstdint>
#include <fstream>
#include <cmath>
#include "glm\mat4x4.hpp"
#include "glm\vec3.hpp"
#include "glm\vec4.hpp"
#include "glm\gtx\transform.hpp"
#include "stb_image\stb_image.h"

#include "RenderSystem.h"
#include "Global.h"
#include "Error.h"
#include "FirstPersonCamera.h"
#include "CinematicCamera.h"
#include "Scene.h"
#include "Lights.h"
#include "Skybox.h"
#include "glad/glad.h"

void GLAPIENTRY
MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
  if (severity != 0x826b)
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
      (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
      type, severity, message);
}

RenderSystem::RenderSystem() : aspect((float)GLOBAL::WINDOW::WIDTH / (float)GLOBAL::WINDOW::HEIGHT),
  nearPlane(0.1f), farPlane(60.0f) {
  UBO = 0;
  projectionMatrix = glm::mat4(0.0f);
  
  float w2 = GLOBAL::WINDOW::WIDTH / 2.0f;
  float h2 = GLOBAL::WINDOW::HEIGHT / 2.0f;
  viewportMatrix = glm::mat4(w2, 0.0f, 0.0f, 0.0f,
    0.0f, h2, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    w2 + 0, h2 + 0, 0.0f, 1.0f);
}

void RenderSystem::StartUp() {
  CreateSDLWindowAndContext();
  InitializeGLADandOpenGL();
  InitializeWindow();

  LoadShaders();
  postProcessing.Load();

  stbi_set_flip_vertically_on_load(true);

  SDL_SetWindowPosition(window, -1921, 200); // todo DEBUG:: Open window on another screen
}

void RenderSystem::ShutDown() {
  postProcessing.Delete();
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void RenderSystem::Render() {
  directionalShadowProgram.Use();
  sceneManager.DrawShadowSceneInstance(directionalShadowProgram, LAYERTYPE::SCENE);

  CheckAndClearViewport();
  UpdateViewProjectionMatrix();

  postProcessing.BindMultiSampledFramebuffer();

  pbrProgram.Use();
  reflectionProbe.SendShaderDataIBL(0, pbrProgram);
  sceneManager.Draw(pbrProgram);

  skyboxProgram.Use();
  sceneManager.DrawSkybox(skyboxProgram);

  refractionProgram.Use();
  sceneManager.DrawWindow(refractionProgram);

  postProcessing.ClearSecondBufferForBloom();

  lampProgram.Use();
  sceneManager.DrawLights(lampProgram);

  postProcessing.BlitMultiSampledFramebuffer();

  gaussianBlurProgram.Use();
  postProcessing.CreateBloomTexture(gaussianBlurProgram);

  postProcessingProgram.Use();
  postProcessing.Draw(postProcessingProgram);

  SDL_GL_SwapWindow(window);
}

void RenderSystem::LateStartUp() {
  SetViewMatrixAndFOV();

  Scene* scene = ((Scene*)gameObjects[static_cast<int>(ID::SCENE)]);
  scene->AttachQueryProgram(&queryProgram);

  for (int i = 0; i < Model::textureHandles.size(); ++i)
    allTextures.textures[i] = Model::textureHandles[i];

  glGenBuffers(1, &UBO);
  glBindBuffer(GL_UNIFORM_BUFFER, UBO);
  glBufferStorage(GL_UNIFORM_BUFFER, Model::textureHandles.size() * sizeof(GLuint64), &allTextures.textures, GL_MAP_READ_BIT);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  const int bindingIndex = 0;
  glBindBufferBase(GL_UNIFORM_BUFFER, bindingIndex, UBO);

  sceneManager.AttachGameObjectsToSceneLayers(gameObjects, viewMatrix, &projectionMatrix);
  sceneManager.AttachViewportMatrix(&viewportMatrix);

  directionalShadowProgram.Use();
  sceneManager.DrawShadowSceneInstance(directionalShadowProgram, LAYERTYPE::SCENE);
  sceneManager.DrawShadowSceneInstance(directionalShadowProgram, LAYERTYPE::ROOM_C);
  CheckAndClearViewport();

  Skybox* skybox = ((Skybox*)gameObjects[static_cast<int>(ID::SKYBOX)]);
  skybox->ConvertEquirectangularEnvironmentMapToCubeMap(equirectangualrToCubeMapProgram);

  ShaderProgram* programs[4] = {
    &reflectionProbeProgram,
    &convolutionProgram,
    &prefilterProgram,
    &bdrfProgram
  };
  reflectionProbe.Load(programs, gameObjects, sceneManager);
}

void RenderSystem::AttachGameObjects(GameObject** gameObjects) {
  this->gameObjects = gameObjects;
}

void RenderSystem::SetViewMatrixAndFOV() {
  FirstPersonCamera* camera = ((FirstPersonCamera*)gameObjects[static_cast<int>(ID::PLAYER_CAMERA)]);
  viewMatrix = camera->GetRefrenceToViewMatrix();
  fov = camera->GetRefrenceToFOV();
}

void RenderSystem::CheckAndClearViewport() {
  if (GLOBAL::WINDOW::RESIZE)
    ResizeViewport();

  glViewport(0, 0, GLOBAL::WINDOW::WIDTH, GLOBAL::WINDOW::HEIGHT);
  glClearColor(0.4f, 0.62f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glStencilMask(0x00);
}

void RenderSystem::UpdateViewProjectionMatrix() {
  FirstPersonCamera* camera = ((FirstPersonCamera*)gameObjects[static_cast<int>(ID::PLAYER_CAMERA)]);
  //CinematicCamera* camera = ((CinematicCamera*) gameObjects[ID::CINEMATIC_CAMERA]);
  viewMatrix = camera->GetRefrenceToViewMatrix();

  projectionMatrix = glm::perspective(glm::radians(*fov), aspect, nearPlane, farPlane);
}

void RenderSystem::CreateSDLWindowAndContext() {
  SDL_Init(0);

  if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
    GAME_ERROR::ERROR_MESSAGE("SDL Error", SDL_GetError());
  if (SDL_InitSubSystem(SDL_INIT_TIMER) < 0)
    GAME_ERROR::ERROR_MESSAGE("SDL Error", SDL_GetError());
  if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
    GAME_ERROR::ERROR_MESSAGE("SDL Error", SDL_GetError());
  //if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) < 0)
    //GAME_ERROR::ERROR_MESSAGE("SDL Error", SDL_GetError());
  if (SDL_InitSubSystem(SDL_INIT_HAPTIC) < 0)
    GAME_ERROR::ERROR_MESSAGE("SDL Error", SDL_GetError());
  //if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) < 0)
    //GAME_ERROR::ERROR_MESSAGE("SDL Error", SDL_GetError());
  if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
    GAME_ERROR::ERROR_MESSAGE("SDL Error", SDL_GetError());

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

  window = nullptr;
  window = SDL_CreateWindow("Title",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    GLOBAL::WINDOW::WIDTH, GLOBAL::WINDOW::HEIGHT,
    SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if (window == nullptr)
    GAME_ERROR::ERROR_MESSAGE("SDL Error", SDL_GetError());

  context = nullptr;
  context = SDL_GL_CreateContext(window);
  if (context == nullptr)
    GAME_ERROR::ERROR_MESSAGE("SDL Error", SDL_GetError());
}

void RenderSystem::InitializeGLADandOpenGL() {
  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    GAME_ERROR::ERROR_MESSAGE("GLAD Error", "Failed to initialize OpenGL context");

  const GLubyte* vendor = glGetString(GL_VENDOR);
  const GLubyte* renderer = glGetString(GL_RENDERER);
  const GLubyte* version = glGetString(GL_VERSION);
  const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
  GLint major = 0, minor = 0;
  glGetIntegerv(GL_MAJOR_VERSION, &major);
  glGetIntegerv(GL_MAJOR_VERSION, &minor);
  printf("GL Vendor     : %s\n", vendor);
  printf("GL Renderer   : %s\n", renderer);
  printf("GL Version    : %s %d.%d\n", version, major, minor);
  printf("GLSL Version  : %s\n", glslVersion);
  printf("\n");

  GLint numExtensions;
  glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
  for (int i = 0; i < numExtensions; ++i)
    printf("%s\n", glGetStringi(GL_EXTENSIONS, i));
  printf("\n");

  int nrAttributes;
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
  printf("Max vertex attributes: %d\n\n", nrAttributes);

  GLint bufs, samples;
  glGetIntegerv(GL_SAMPLE_BUFFERS, &bufs);
  glGetIntegerv(GL_SAMPLES, &samples);
  printf("MSAA: buffers = %d samples = %d\n", bufs, samples);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_CULL_FACE);
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
  //glEnable(GL_DEBUG_OUTPUT);
  //glDebugMessageCallback(MessageCallback, 0);
}

void RenderSystem::InitializeWindow() {
  if (SDL_GL_SetSwapInterval(0) < 0)
    GAME_ERROR::ERROR_MESSAGE("SDL Error", SDL_GetError());

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void RenderSystem::ResizeViewport() {
  glViewport(0, 0, GLOBAL::WINDOW::WIDTH, GLOBAL::WINDOW::HEIGHT);

  float w2 = GLOBAL::WINDOW::WIDTH / 2.0f;
  float h2 = GLOBAL::WINDOW::HEIGHT / 2.0f;
  viewportMatrix = glm::mat4(w2, 0.0f, 0.0f, 0.0f,
    0.0f, h2, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    w2 + 0, h2 + 0, 0.0f, 1.0f);

  GLOBAL::WINDOW::RESIZE = false;
}

void RenderSystem::LoadShaders() {
  lampProgram.Add(GL_VERTEX_SHADER, "../files/shader/lamp.vert");
  lampProgram.Add(GL_FRAGMENT_SHADER, "../files/shader/lamp.frag");
  lampProgram.Compile();
  lampProgram.Link();

  skyboxProgram.Add(GL_VERTEX_SHADER, "../files/shader/skybox.vert");
  skyboxProgram.Add(GL_FRAGMENT_SHADER, "../files/shader/skybox.frag");
  skyboxProgram.Compile();
  skyboxProgram.Link();

  directionalShadowProgram.Add(GL_VERTEX_SHADER, "../files/shader/directionalShadow.vert");
  directionalShadowProgram.Add(GL_FRAGMENT_SHADER, "../files/shader/directionalShadow.frag");
  directionalShadowProgram.Compile();
  directionalShadowProgram.Link();

  refractionProgram.Add(GL_VERTEX_SHADER, "../files/shader/refraction.vert");
  refractionProgram.Add(GL_FRAGMENT_SHADER, "../files/shader/refraction.frag");
  refractionProgram.Compile();
  refractionProgram.Link();

  blendProgram.Add(GL_VERTEX_SHADER, "../files/shader/blend.vert");
  blendProgram.Add(GL_FRAGMENT_SHADER, "../files/shader/blend.frag");
  blendProgram.Compile();
  blendProgram.Link();

  postProcessingProgram.Add(GL_VERTEX_SHADER, "../files/shader/postProcessing.vert");
  postProcessingProgram.Add(GL_FRAGMENT_SHADER, "../files/shader/postProcessing.frag");
  postProcessingProgram.Compile();
  postProcessingProgram.Link();

  pbrProgram.Add(GL_VERTEX_SHADER, "../files/shader/pbr.vert");
  pbrProgram.Add(GL_GEOMETRY_SHADER, "../files/shader/pbr.geom");
  pbrProgram.Add(GL_FRAGMENT_SHADER, "../files/shader/pbr.frag");
  pbrProgram.Compile();
  pbrProgram.Link();

  equirectangualrToCubeMapProgram.Add(GL_VERTEX_SHADER, "../files/shader/equirectangularToCubemap.vert");
  equirectangualrToCubeMapProgram.Add(GL_FRAGMENT_SHADER, "../files/shader/equirectangularToCubemap.frag");
  equirectangualrToCubeMapProgram.Compile();
  equirectangualrToCubeMapProgram.Link();

  convolutionProgram.Add(GL_VERTEX_SHADER, "../files/shader/convolution.vert");
  convolutionProgram.Add(GL_FRAGMENT_SHADER, "../files/shader/convolution.frag");
  convolutionProgram.Compile();
  convolutionProgram.Link();

  prefilterProgram.Add(GL_VERTEX_SHADER, "../files/shader/prefilter.vert");
  prefilterProgram.Add(GL_FRAGMENT_SHADER, "../files/shader/prefilter.frag");
  prefilterProgram.Compile();
  prefilterProgram.Link();

  bdrfProgram.Add(GL_VERTEX_SHADER, "../files/shader/bdrf.vert");
  bdrfProgram.Add(GL_FRAGMENT_SHADER, "../files/shader/bdrf.frag");
  bdrfProgram.Compile();
  bdrfProgram.Link();

  reflectionProbeProgram.Add(GL_VERTEX_SHADER, "../files/shader/lightProbeCubemap.vert");
  reflectionProbeProgram.Add(GL_FRAGMENT_SHADER, "../files/shader/lightProbeCubemap.frag");
  reflectionProbeProgram.Compile();
  reflectionProbeProgram.Link();

  queryProgram.Add(GL_VERTEX_SHADER, "../files/shader/query.vert");
  queryProgram.Add(GL_FRAGMENT_SHADER, "../files/shader/query.frag");
  queryProgram.Compile();
  queryProgram.Link();

  gaussianBlurProgram.Add(GL_VERTEX_SHADER, "../files/shader/gaussianBlur.vert");
  gaussianBlurProgram.Add(GL_FRAGMENT_SHADER, "../files/shader/gaussianBlur.frag");
  gaussianBlurProgram.Compile();
  gaussianBlurProgram.Link();
}