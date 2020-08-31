#include "Switches.h"

Switches::Switches() : GameObject() {
  for (int i = 0; i < numSwitches; ++i)
    switchIsOn[i] = false;
  switchesToggled = false;
}

void Switches::Load() {
  for (unsigned i = 0; i < numSwitches; ++i) {
    models[i].LoadModel("../files/asset/switches/switch_animated_" + std::to_string(i) + ".dae", false);
    models[i].NextAnimation();
    switchIsOn[i] = true;
  }

  switchesToggled = false;
}

void Switches::Update(float dt) {
  for (unsigned i = 0; i < numSwitches; ++i)
    models[i].Update(dt);

  //printf("%d%d%d\n", switchIsOn[0], switchIsOn[1], switchIsOn[2]);
}

void Switches::Draw(ShaderProgram& shaderProgram, const glm::mat4& projectionMatrix, 
  const glm::mat4& viewMatrix, const int index) {
  
  const int sceneIndex = 0;
  if (index == sceneIndex) {
    for (int i = 0; i < numSwitches; ++i) {
      const glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
      const glm::mat4 modelViewMatrix = viewMatrix * (modelMatrix);
      const glm::mat4 modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;

      shaderProgram.SetUniformMat4("modelMatrix", glm::mat4(1.0f));
      shaderProgram.SetUniformMat3("normalMatrix", normalMatrix);
      shaderProgram.SetUniformMat4("viewProjectionMatrix", projectionMatrix * (viewMatrix));
      shaderProgram.SetUniformMat4("modelViewProjectionMatrix", modelViewProjectionMatrix);

      shaderProgram.SetUniformBool("isAnimated", true);
      shaderProgram.SetUniformMat4Array("jointTransforms", models[i].GetJointCount(), models[i].GetJointTransforms());

      models[i].Draw();
    }
  }
}

void Switches::Delete() {}

void Switches::SendShaderData(ShaderProgram& shaderProgram, const int index) {
  shaderProgram.SetUniformMat4Array("jointTransforms", models[index].GetJointCount(),
    models[index].GetJointTransforms());
}

const unsigned Switches::NumElements() { return 3; }

bool Switches::AnimationCompleteDoorIsOpen(int doorIndex) {
  return models[doorIndex].AnimationComplete() && switchIsOn[doorIndex];
}

bool Switches::AnimationIsPercentCompleteDoorIsOpen(int doorIndex, const float percent) {
  return models[doorIndex].AnimationIsPercentComplete(percent) && switchIsOn[doorIndex];
}

void Switches::NextAnimation(int doorIndex) {
  if (models[doorIndex].AnimationComplete())
    models[doorIndex].NextAnimation();
}

void Switches::ToggleLeftSwitch() {
  if (models[0].AnimationComplete()) {
    switchIsOn[0] = !switchIsOn[0];
    models[0].NextAnimation();
  }
}

void Switches::ToggleCenterSwitch() {
  if (models[1].AnimationComplete()) {
    switchIsOn[1] = !switchIsOn[1];
    models[1].NextAnimation();
  }
}

void Switches::ToggleRightSwitch() {
  if (models[2].AnimationComplete()) {
    switchIsOn[2] = !switchIsOn[2];
    models[2].NextAnimation();
  }
}

bool Switches::IsLeftSwitchOn() { return switchIsOn[0]; }

bool Switches::IsCenterSwitchOn() { return switchIsOn[1]; }

bool Switches::IsRightSwitchOn() { return switchIsOn[2]; }

void Switches::SetSwitchToggleFlag(bool switchesToggled) { this->switchesToggled = switchesToggled; }

bool Switches::SwitchesToggled() { return switchesToggled; }