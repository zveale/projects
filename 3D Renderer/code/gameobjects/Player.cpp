#include "Player.h"
#include "GameObject.h"
#include "Global.h"
#include "FirstPersonCamera.h"
#include "ShaderProgram.h"
#include "glm\vec3.hpp"
#include "Lights.h"

Player::Player() : GameObject() {}

void Player::Load() {
  position = glm::vec3(0.0f, 0.0f, 0.0f);
  front = glm::vec3(0.0f, 0.0f, -1.0f);
  right = glm::vec3(0.0f, 0.0f, 0.0f);
  speed = 0.0f;
  yPosition = 2.0f;
}

void Player::Update(float dt) {
  glm::vec3 offset = glm::vec3(0.0f, 0.0f, -0.2f);
  light->position = position + glm::normalize(front) * -1.0f;
  light->direction = front;

  light->cutoff = glm::cos(glm::radians(8.0f));
  light->outerCutoff = glm::cos(glm::radians(60.5f));
}

void Player::Draw(ShaderProgram& shaderProgram, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const int index) {}

void Player::Delete() {}

const unsigned Player::NumElements() { return 1; }

void Player::AttachCamera(FirstPersonCamera& camera) {
  camera.SetTarget(&position, &front, &right);
}

void Player::SendDataShader(ShaderProgram& shaderProgram) {}

void Player::SendShaderData(ShaderProgram& shaderProgram, const int index) {}

void Player::MaintainHeight() { position.y = yPosition; }

void Player::Crouch() { yPosition = 0.9f; }

void Player::Stand() { yPosition = 1.0f; }

void Player::Walk() { speed = 1.0f; }

void Player::Sprint() { speed = 4.0f; }

void Player::Elevate(const float dt) { position.y += (1.0f * dt); }

void Player::MoveUp(const float dt) { position += front * (speed * dt); }

void Player::MoveDown(const float dt) { position -= front * (speed * dt); }

void Player::MoveLeft(const float dt) { position -= right * (speed * dt); }

void Player::MoveRight(const float dt) { position += right * (speed * dt); }

void Player::AttachLight(Light* light) {
  this->light = light;
}