#ifndef WINDOW_H
#define WINDOW_H

#include <vector>
#include "GameObject.h"
#include "StaticModel.h"
#include "ShaderProgram.h"
#include "glm\mat4x4.hpp"

class Window : public GameObject {
public:
  Window();
  void Load();
  void Update(float dt = 0.0f);
  void Draw(ShaderProgram& shaderProgram, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const int index = 0);
  void Delete();
  void SendShaderData(ShaderProgram& shaderProgram, const int index = 0);
  const unsigned NumElements();

private:
  StaticModel model;
};

#endif // !WINDOW_H
