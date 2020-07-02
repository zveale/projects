#include "Elevator.h"
#include "GameObject.h"
#include "StaticModel.h"

Elevator::Elevator() : GameObject() { glm::vec3(0.0f); }

void Elevator::Load() {
  model.LoadModel("../files/asset/level/elevator.dae", false);
  position = glm::vec3(0.0f, -40.0f, 0.0f);
}

void Elevator::Update(float dt) {
  modelMatrix = glm::mat4(1.0f);
  modelMatrix = glm::translate(modelMatrix, position);
}

void Elevator::Draw(ShaderProgram& shaderProgram, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const int index) {
  const glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
  const glm::mat4 viewProjectionMatrix = projectionMatrix * glm::mat4(glm::mat3(viewMatrix));
  const glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
  const glm::mat4 modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;
  shaderProgram.SetUniformMat4("modelMatrix", modelMatrix);
  shaderProgram.SetUniformMat3("normalMatrix", normalMatrix);
  shaderProgram.SetUniformMat4("viewMatrix", viewMatrix);
  shaderProgram.SetUniformMat4("projectionMatrix", projectionMatrix);
  shaderProgram.SetUniformMat4("viewProjectionMatrix", viewProjectionMatrix);
  shaderProgram.SetUniformMat4("modelViewProjectionMatrix", modelViewProjectionMatrix);

  shaderProgram.SetUniformBool("isAnimated", false);

  model.Draw();
}

void Elevator::Delete() {}

void Elevator::SendShaderData(ShaderProgram& shaderProgram, const int index) {}

const unsigned Elevator::NumElements() { return 1; }

void Elevator::MaintainHeight() { position.y = 0.0f; }

void Elevator::Elevate(const float dt) { position.y += (1.0f * dt); }