#ifndef ELEVATOR_H
#define ELEVATOR_H

#include "GameObject.h"
#include "StaticModel.h"

class Elevator : public GameObject {
public:
  Elevator();
  void Load();
  void Update(float dt = 0.0f);
  void Draw(ShaderProgram& shaderProgram, int index = 0);
  void Delete();
  void SendShaderData(ShaderProgram& shaderProgram, const int index = 0);
  const unsigned NumElements();

  void MaintainHeight();

  /*
  Called in the logic system to raise the elevator up.
  */
  void Elevate(const float dt);

  StaticModel* GetModel();
  glm::mat4* GetMatrix();

private:
  StaticModel staticModel;
  glm::vec3 position;
};

#endif // !ELEVATOR_H
