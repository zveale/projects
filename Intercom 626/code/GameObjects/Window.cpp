#include "Window.h"

Window::Window() : GameObject() {}

void Window::Load() { staticModel.LoadModel("../files/asset/level/windows.dae"); }

void Window::Update(float dt) {}

void Window::Draw(ShaderProgram& shaderProgram, int index) {
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  staticModel.Draw(shaderProgram);
  glDisable(GL_BLEND);
}

void Window::Delete() {}

const unsigned Window::NumElements() { return 1; }

void Window::SendShaderData(ShaderProgram& shaderProgram, const int index) {}

