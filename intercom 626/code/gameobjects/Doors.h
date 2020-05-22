#ifndef DOORS_H
#define DOORS_H

#include <string>
#include "GameObject.h"
#include "AnimatedModel.h"
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

class Doors : public GameObject {
public:
  Doors();
  void Load();
  void Update(float dt = 0.0f);
  void Draw(ShaderProgram& shaderProgram, int index = 0);
  void Delete();
  void SendShaderData(ShaderProgram& shaderProgram, const int index = 0);
  const unsigned NumElements();

  bool AnimationCompleteDoorIsOpen(int doorIndex);
  bool AnimationIsPercentCompleteDoorIsOpen(int doorIndex, const float percent);
  void NextAnimation(int doorIndex);

  /*
  Called in Logic system.
  */
  bool isDoorOpen(const int index);
  void OpenDoor(const int index);
  void CloseDoor(const int index);
  void CloseAllDoors();

  glm::vec3 GetPosition(int index);

  AnimatedModel** GetModels();
  glm::vec3** GetPositions();

private:
  const static unsigned numDoors = 10; 
  AnimatedModel models[numDoors];
  glm::vec3 positions[numDoors];
  AnimatedModel* modelRefrences[numDoors];
  glm::vec3* positionRefrences[numDoors];
  bool doorIsOpen[numDoors];

  void LoadPositions(std::string path);
  void ProcessSceneGraph(aiNode* node, const aiScene* scene);

  /*
  Only used on loading models.
  */
  void GetPosition(const aiNode* node, const aiScene* scene);

  glm::mat4 Cast_aiMat4ToGlmMat4(const aiMatrix4x4& ai_matr);
};

#endif // !DOORS_H