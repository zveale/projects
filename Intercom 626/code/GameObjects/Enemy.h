#ifndef ENEMY_H
#define ENEMY_H

#include <cstdio>
#include "glm\vec3.hpp"
#include "glm\gtc\quaternion.hpp"
#include "glm\mat4x4.hpp"
#include "GameObject.h"
#include "Pathfinding.h"
#include "StaticModel.h"
#include "AnimatedModel.h"
#include "Player.h"

class Enemy: public GameObject {
public:
  Enemy();
  void Load();
  /*
  Update the path to player every 10 seconds. Move and rotate towards the player if it is close
  */
  void Update(float dt = 0.0f);
  void Draw(ShaderProgram& shaderProgram, int index = 0);
  void SendShaderData(ShaderProgram& shaderProgram, const int index = 0);
  void Delete();
  const unsigned NumElements();

  void NextAnimation();
  void SelectAnimation(const int poseIndex);
  void CurrentAnimation();

  bool EnemyAtTarget(const glm::vec3& position, const glm::vec3& target, const float speed, const float dt);
  glm::vec3 MoveTowards(const glm::vec3& position, const glm::vec3& target, const float speed, const float dt);
  glm::quat RotateTowards(const glm::quat& rotation, const glm::vec3& position, const glm::vec3& target, const float speed, const float dt);

  void AttachPlayer(const Player& player);

  AnimatedModel* GetModel();
  glm::mat4* GetMatrix();

private:
  glm::vec3 position;
  glm::vec3 targetPosition;
  glm::quat rotation;
  glm::vec3 player;
  float speed;
  float rotationSpeed;

  Pathfinding pathfinding;
  AnimatedModel model;

  float time;
  int seconds;

  const glm::vec3* playerPosition;
  const glm::vec3* playerFront;
  const glm::vec3* playerRight;
  float refreshTime;
  int x;
};

#endif ENEMY_H