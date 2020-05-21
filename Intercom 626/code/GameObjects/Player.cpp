#include "Player.h"
#include "GameObject.h"
#include "Global.h"
#include "FirstPersonCamera.h"
#include "ShaderProgram.h"
#include "glm\vec3.hpp"
#include "Lights.h"

Player::Player() : GameObject() {}

void Player::Load() {
  position = glm::vec3(0.0f, -38.0f, 0.0f);
  front = glm::vec3(0.0f, 0.0f, -1.0f);
  right = glm::vec3(0.0f, 0.0f, 0.0f);
  speed = 0.0f;
  yPosition = 2.0f;
}

void Player::Update(float dt) {
  glm::vec3 offset = glm::vec3(0.2f, 0.0f, -0.1f);
  light->position = position + offset;
  glm::vec3 forward = position + front;
  light->direction = front;
  light->cutoff = glm::cos(glm::radians(8.0f));
  light->outerCutoff = glm::cos(glm::radians(60.5f));

  // fixme temp
  light->position = glm::vec3(0.0f, 0.0f, 0.0f);
}

void Player::Draw(ShaderProgram& shaderProgram, int index) {}

void Player::Delete() {}

const unsigned Player::NumElements() { return 1; }

void Player::AttachCamera(FirstPersonCamera& camera) {
  camera.SetTarget(&position, &front, &right);
}

void Player::SendDataShader(ShaderProgram& shaderProgram) {
  shaderProgram.SetUniformVec3("viewPosition", position);
  shaderProgram.SetUniformVec4("lightPosition", glm::vec4(light->position, 1.0f));
}

void Player::SendShaderData(ShaderProgram& shaderProgram, const int index) {}

void Player::MaintainHeight() { position.y = yPosition; }

void Player::Crouch() { yPosition = 0.9f; }

void Player::Stand() { yPosition = 1.0f; }

void Player::Walk() { speed = 2.0f; }

void Player::Sprint() { speed = 10.0f; }

void Player::Elevate(const float dt) { position.y += (1.0f * dt); }

void Player::MoveUp(const float dt) { position += front * (speed * dt); }

void Player::MoveDown(const float dt) { position -= front * (speed * dt); }

void Player::MoveLeft(const float dt) { position -= right * (speed * dt); }

void Player::MoveRight(const float dt) { position += right * (speed * dt); }

void Player::AttachLight(Light* light) {
  this->light = light;
}