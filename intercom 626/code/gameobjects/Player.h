#ifndef PLAYER_H
#define PLAYER_H

#include "GameObject.h"
#include "Global.h"
#include "FirstPersonCamera.h"
#include "ShaderProgram.h"
#include "glm\vec3.hpp"
#include "Lights.h"

class Player : public GameObject {
public:
  Player();
  void Load();

  /*
  Update light attached to player.
  */
  void Update(float dt = 0.0f);

  void Draw(ShaderProgram& shaderProgram, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const int index = 0);
  void Delete();
  const unsigned NumElements();

  void AttachCamera(FirstPersonCamera& camera);
  void AttachLight(Light* light);

  void SendDataShader(ShaderProgram& shaderProgram);
  void SendShaderData(ShaderProgram& shaderProgram, const int index = 0);

  /*
  Called in logic system.
  */
  void MaintainHeight();
  void Crouch();
  void Stand();
  void Walk();
  void Sprint();
  void Elevate(const float dt);
  void MoveUp(const float dt);
  void MoveDown(const float dt);
  void MoveLeft(const float dt);
  void MoveRight(const float dt);

  glm::vec3 position;
  glm::vec3 front;
  glm::vec3 right;
  float speed;
  float yPosition;
  Light* light;
};

#endif // !PLAYER_H

