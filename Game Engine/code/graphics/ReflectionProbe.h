// Learn OpenGL.Specular - IBL: https://learnopengl.com/PBR/IBL/Specular-IBL
// Chetan Jags : Image Based Lighting https : https://chetanjags.wordpress.com/2015/08/26/image-based-lighting/

#ifndef REFLECTIONPROBE_H
#define REFLECTIONPROBE_H

#include <vector>
#include "glad\glad.h"
#include "glm\vec3.hpp"
#include "glm\mat4x4.hpp"
#include "glm\gtx\transform2.hpp"
#include "ShaderProgram.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "Skybox.h"
#include "Lights.h"

class ReflectionProbe {
public:
  void Load(ShaderProgram** shaderProgram, GameObject** gameObjects, SceneManager& sceneManager);
  void UnbindFramebuffer();
  unsigned NumLightProbes();
  const GLuint GetCubemapTextureId(const int index) const;
  void SendShaderDataIBL(const int index, ShaderProgram& shaderProgram);

private:
  struct Probe {
    Probe(const glm::vec3& postition);
    glm::vec3 position;

    GLuint cubemapTextureId, irradianceTextureId, prefilterTextureId, brdfIntegrationTextureId;
  };
  std::vector<Probe> probes;

  const GLuint textureCubemapLocation = 48, textureIrradianceLocation = 47,
    texturePrefilterLocation = 46, textureBdrfLocation = 45;

  GLuint vao, quadVAO;

  void SetupBuffer();
  void CreateCubeMap(const int index, ShaderProgram& shaderProgram, GameObject** gameObjects, SceneManager& sceneManager);
  void CreateIrradianceMap(const int index, ShaderProgram& shaderProgram);
  void CreatePrefilterMap(const int index, ShaderProgram& shaderProgram);
  void CreatebrdfIntegrationMap(const int index, ShaderProgram& shaderProgram);
};

#endif // !REFLECTIONPROBE_H