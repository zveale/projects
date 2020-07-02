#include <cstdio>
#include "Enemy.h"
#include "glm\vec3.hpp"
#include "glm\gtc\quaternion.hpp"
#include "glm\mat4x4.hpp"
#include "GameObject.h"
#include "Pathfinding.h"
#include "StaticModel.h"
#include "AnimatedModel.h"
#include "Player.h"

Enemy::Enemy() : GameObject() {
  position =  glm::vec3(0.0f);
  targetPosition = glm::vec3(0.0f);
  player = glm::vec3(0.0f);

  speed = 0.0f;
  rotationSpeed = 0.0f;
  time = 0.0f;
  seconds = 0;
  refreshTime = 0.0f;
  x = 0;
}

void Enemy::Load() {
  position = glm::vec3(10.0f, 0.2f, 10.0f);
  targetPosition = glm::vec3(0.0f, 0.0f, 0.0f);

  speed = 1.3f;
  rotationSpeed = 1.1f;
  time = 0.0f;
  seconds = 0;
  x = 0;
  pathfinding.Load();

  model.LoadModel("../files/asset/enemy/HorrorMonster.dae", false);

  player = glm::vec3(20.0f, 0.0f, 0.0f);
  pathfinding.FindPath(position, player);
  targetPosition = pathfinding.FirstPosition();
  targetPosition.y = 0.2f;

  seconds = 0;
  speed = 1.1f;
}


void Enemy::Update(float dt) {
  time += dt;
  if (time >= 1.0f) {
    time = 0.0f;
    ++seconds;
  }

  if (seconds >= 10) {
    player = *playerPosition;
    player.y = 0.2f;
    pathfinding.FindPath(position, player);
    targetPosition = pathfinding.FirstPosition();
    targetPosition.y = 0.2f;

    seconds = 0;
    speed = 1.3f;
  }

  if (EnemyAtTarget(position, targetPosition, speed, dt)) {
    position = targetPosition;
    targetPosition = pathfinding.FirstPosition();
    targetPosition.y = 0.2f;
  }

  if (position != player) {
    float distanceToPlayer = glm::distance(position, player);
    if (distanceToPlayer < 1.0f)
      if (player != position)
        targetPosition = player;
    if (distanceToPlayer < 1.0f)
      speed = 0.0f;
  }

  position = MoveTowardsTarget(position, targetPosition, speed, dt);
  rotation = RotateTowardsTarget(rotation, position, targetPosition, rotationSpeed, dt);

  modelMatrix = glm::mat4(1.0f);
  modelMatrix = glm::translate(modelMatrix, position);
  modelMatrix = modelMatrix * glm::mat4_cast(rotation);
  modelMatrix = glm::scale(modelMatrix, glm::vec3(0.8f));

  model.Update(dt);
}

void Enemy::Draw(ShaderProgram& shaderProgram, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const int index) {
  const glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
  const glm::mat4 modelViewMatrix = viewMatrix * (modelMatrix);
  const glm::mat4 modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;

  shaderProgram.SetUniformMat4("modelMatrix", modelMatrix);
  shaderProgram.SetUniformMat3("normalMatrix", normalMatrix);
  shaderProgram.SetUniformMat4("viewProjectionMatrix", projectionMatrix * viewMatrix);
  shaderProgram.SetUniformMat4("modelViewProjectionMatrix", modelViewProjectionMatrix);

  shaderProgram.SetUniformBool("isAnimated", true);
  shaderProgram.SetUniformMat4Array("jointTransforms", model.GetJointCount(), model.GetJointTransforms());
  
  model.Draw();
}

void Enemy::NextAnimation() {
  if (model.AnimationComplete())
    model.NextAnimation();
}

void Enemy::SelectAnimation(const int poseIndex) {
  if (model.AnimationComplete())
    model.SelectAnimation(poseIndex);
}

void Enemy::CurrentAnimation() {
  if (model.AnimationComplete())
    model.CurrentAnimation();
}

void Enemy::Delete() {}

const unsigned Enemy::NumElements() { return 1; }

void Enemy::SendShaderData(ShaderProgram& shaderProgram, const int index) {
  shaderProgram.SetUniformMat4Array("jointTransforms", model.GetJointCount(), model.GetJointTransforms());
  shaderProgram.SetUniformMat4("modelMatrix", modelMatrix);
}

bool Enemy::EnemyAtTarget(const glm::vec3& position, const glm::vec3& target, const float speed, const float dt) {
  float magnitude = glm::length(target - position);
  float distance = speed * dt;

  if (magnitude <= distance || magnitude == 0.0f)
    return true;

  return false;
}

glm::vec3 Enemy::MoveTowardsTarget(const glm::vec3& position, const glm::vec3& target, const float speed, const float dt) {
  glm::vec3 direction = target - position;
  float magnitude = glm::length(direction);
  float distance = speed * dt;

  if (magnitude <= distance || magnitude == 0.0f) {
    return position;
  }

  return position + (direction / magnitude) * distance;
}

glm::quat Enemy::RotateTowardsTarget(const glm::quat& rotation, const glm::vec3& position, const glm::vec3& target, const float speed, const float dt) {
  if (target != position) {
    glm::vec3 direction = glm::normalize(target - position);
    glm::quat nextRotation = glm::quatLookAtLH(direction, glm::vec3(0.0f, 1.0f, 0.0f));
    return glm::slerp(rotation, nextRotation, speed * dt);
  }
  else
    return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
}

void Enemy::AttachPlayer(const Player& player) {
  playerPosition = &player.position;
  playerFront = &player.front;
  playerRight = &player.right;
}

const glm::vec3& Enemy::GetPosition() const { return position; }
