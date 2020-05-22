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

Enemy::Enemy() : GameObject() {}

void Enemy::Load() {
  position = glm::vec3(0.0f, 0.2f, 0.0f);
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
  targetPosition = pathfinding.NextPosition();
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
    targetPosition = pathfinding.NextPosition();
    targetPosition.y = 0.2f;

    seconds = 0;
    speed = 1.3f;
  }

  if (EnemyAtTarget(position, targetPosition, speed, dt)) {
    position = targetPosition;
    targetPosition = pathfinding.NextPosition();
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

  position = MoveTowards(position, targetPosition, speed, dt);
  position = glm::vec3(0.0f, 10.2f, 0.0f);
  rotation = RotateTowards(rotation, position, targetPosition, rotationSpeed, dt);

  modelMatrix = glm::mat4(1.0f);
  modelMatrix = glm::translate(modelMatrix, position);
  modelMatrix = modelMatrix * glm::mat4_cast(rotation);
  modelMatrix = glm::scale(modelMatrix, glm::vec3(0.8f));

  model.Update(dt);
}

void Enemy::Draw(ShaderProgram& shaderProgram, int index) {
  model.Draw(shaderProgram);
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

bool Enemy::EnemyAtTarget(const glm::vec3& position, const glm::vec3& target, const float speed, const float dt) {
  float magnitude = glm::length(target - position);
  float distance = speed * dt;

  if (magnitude <= distance || magnitude == 0.0f)
    return true;

  return false;
}

const unsigned Enemy::NumElements() { return 1; }

void Enemy::SendShaderData(ShaderProgram& shaderProgram, const int index) {
  shaderProgram.SetUniformMat4Array("jointTransforms", model.GetJointCount(), model.GetJointTransforms());
  shaderProgram.SetUniformMat4("modelMatrix", modelMatrix);
}

glm::vec3 Enemy::MoveTowards(const glm::vec3& position, const glm::vec3& target, const float speed, const float dt) {
  glm::vec3 direction = target - position;
  float magnitude = glm::length(direction);
  float distance = speed * dt;

  if (magnitude <= distance || magnitude == 0.0f) {
    return position;
  }

  return position + (direction / magnitude) * distance;
}

glm::quat Enemy::RotateTowards(const glm::quat& rotation, const glm::vec3& position, const glm::vec3& target, const float speed, const float dt) {
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

AnimatedModel* Enemy::GetModel() { return &model; }
glm::mat4* Enemy::GetMatrix() { return &modelMatrix; }