#include "Elevator.h"
#include "GameObject.h"
#include "StaticModel.h"

Elevator::Elevator() : GameObject() {}

void Elevator::Load() {
  staticModel.LoadModel("../files/asset/level/elevator.dae", false);
  position = glm::vec3(0.0f, -40.0f, 0.0f);
}

void Elevator::Update(float dt) {
  modelMatrix = glm::mat4(1.0f);
  modelMatrix = glm::translate(modelMatrix, position);
}

void Elevator::Draw(ShaderProgram& shaderProgram, int index) {}
void Elevator::Delete() {}
void Elevator::SendShaderData(ShaderProgram& shaderProgram, const int index) {}

const unsigned Elevator::NumElements() { return 1; }

void Elevator::MaintainHeight() { position.y = 0.0f; }

void Elevator::Elevate(const float dt) { position.y += (1.0f * dt); }

StaticModel* Elevator::GetModel() { return &staticModel; }
glm::mat4* Elevator::GetMatrix() { return &modelMatrix; }