#ifndef CAMERA_H
#define CAMERA_H

#include "GameObject.h"
#include "glad\glad.h"
#include "glm\vec3.hpp"
#include "glm\gtc\matrix_transform.hpp"

class Camera: public GameObject {
public:
  void Load();

  /*
  Inherited by first person and cinematic camera.
  */
  virtual void Update(float dt = 0.0f) = 0;
  
  void Draw(ShaderProgram& shaderProgram, int index = 0);
  void Delete();
  void SendShaderData(ShaderProgram& shaderProgram, const int index = 0);
  const unsigned NumElements();

  glm::mat4* GetRefrenceToViewMatrix();
  float* GetRefrenceToFOV();

protected:
  glm::vec3 position;
  glm::vec3 target;
  glm::vec3 up;
  float yaw;
  float pitch;
  float speed;
  float sensitivity;
  float zoom;
  glm::mat4 viewMatrix;
};

#endif  // !CAMERA_H