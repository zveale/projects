#include "Camera.h"
#include "GameObject.h"
#include "glad\glad.h"
#include "glm\vec3.hpp"
#include "glm\gtc\matrix_transform.hpp"

  void Camera::Load() {
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    target = glm::vec3(0.0f, 0.0f, 0.0f);
    up = glm::vec3(0.0f, 1.0f, 0.0f);
    yaw = -90.0f;
    pitch = 0.0f;
    speed = 2.5f;
    sensitivity = 0.1f;
    zoom = 60.0f;
    viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), target, up);
  }

  void Camera::Draw(ShaderProgram& shaderProgram, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const int index) {}

  void Camera::Delete() {}

  void Camera::SendShaderData(ShaderProgram& shaderProgram, const int index) {}

  const unsigned Camera::NumElements() { return 1; }

  glm::mat4* Camera::GetRefrenceToViewMatrix() { return &viewMatrix; }

  float* Camera::GetRefrenceToFOV() { return &zoom; }