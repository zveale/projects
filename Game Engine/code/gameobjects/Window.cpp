#include "Window.h"

Window::Window() : GameObject() {}

void Window::Load() { model.LoadModel("../files/asset/level/windows.dae"); }

void Window::Update(float dt) {}

void Window::Draw(ShaderProgram& shaderProgram, const glm::mat4& projectionMatrix, 
  const glm::mat4& viewMatrix, const int index) {
  const glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
  const glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
  const glm::mat4 modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;

  shaderProgram.SetUniformMat4("modelMatrix", modelMatrix);
  shaderProgram.SetUniformMat3("normalMatrix", normalMatrix);
  shaderProgram.SetUniformMat4("projectionMatrix", projectionMatrix);
  shaderProgram.SetUniformMat4("modelViewProjectionMatrix", modelViewProjectionMatrix);
  
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  model.Draw();
  glDisable(GL_BLEND);
}

void Window::Delete() {}

const unsigned Window::NumElements() { return 1; }

void Window::SendShaderData(ShaderProgram& shaderProgram, const int index) {}

