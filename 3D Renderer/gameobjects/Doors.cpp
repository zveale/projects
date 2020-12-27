#include <string>
#include "Doors.h"

Doors::Doors() : GameObject() {
  for (int i = 0; i < numDoors; ++i) {
    positions[i] = glm::vec3(0.0f);
    doorIsOpen[i] = false;
  }
}

void Doors::Load() {
  for (unsigned i = 0; i < numDoors; ++i) {
    models[i].LoadModel("../files/asset/doors/door_animated_" + std::to_string(i) + ".dae", false);
    doorIsOpen[i] = false;
  }

  LoadPositions("../files/asset/doors/door_positions.dae");
}

void Doors::Update(float dt) {
  for (unsigned i = 0; i < numDoors; ++i)
    models[i].Update(dt);
}

void Doors::SendShaderData(ShaderProgram& shaderProgram, const int index) {
  shaderProgram.SetUniformMat4Array("jointTransforms", models[index].GetJointCount(),
    models[index].GetJointTransforms());
}

void Doors::Draw(ShaderProgram& shaderProgram, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const int index) {
  const glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
  const glm::mat4 modelViewMatrix = viewMatrix * (modelMatrix);
  const glm::mat4 modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;

  shaderProgram.SetUniformMat4("modelMatrix", glm::mat4(1.0f));
  shaderProgram.SetUniformMat3("normalMatrix", normalMatrix);
  shaderProgram.SetUniformMat4("viewProjectionMatrix", projectionMatrix * (viewMatrix));
  shaderProgram.SetUniformMat4("modelViewProjectionMatrix", modelViewProjectionMatrix);

  shaderProgram.SetUniformBool("isAnimated", true);
  shaderProgram.SetUniformMat4Array("jointTransforms", models[index].GetJointCount(), models[index].GetJointTransforms());
  
  models[index].Draw();
}

void Doors::Delete() {}

const unsigned Doors::NumElements() { return numDoors; }

bool Doors::AnimationCompleteDoorIsOpen(int doorIndex) {
  return models[doorIndex].AnimationComplete() && doorIsOpen[doorIndex];
}

bool Doors::AnimationIsPercentCompleteDoorIsOpen(int doorIndex, const float percent) {
  return models[doorIndex].AnimationIsPercentComplete(percent) && doorIsOpen[doorIndex];
}

void Doors::NextAnimation(int doorIndex) {
  if (models[doorIndex].AnimationComplete())
    models[doorIndex].NextAnimation();
}

bool Doors::isDoorOpen(const int index) { return doorIsOpen[index]; }

void Doors::OpenDoor(const int index) {
  if (!doorIsOpen[index] && models[index].AnimationComplete()) {
    models[index].NextAnimation();
    doorIsOpen[index] = true;
  }
}

void Doors::CloseDoor(const int index) {
  if (doorIsOpen[index] && models[index].AnimationComplete()) {
    models[index].NextAnimation();
    doorIsOpen[index] = false;
  }
}

void Doors::CloseAllDoors() {
  for (int i = 0; i < numDoors; ++i) {
    if (doorIsOpen[i]) {
      models[i].NextAnimation();
      doorIsOpen[i] = false;
    }
  }
}

glm::vec3 Doors::GetPosition(int index) { return glm::vec3(0.0f); }

void Doors::LoadPositions(std::string path) {
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(path, NULL);

  if (!scene || !scene->mRootNode) {
    printf("Error loading shadows!\n Error message: %s\n", importer.GetErrorString());
    return;
  }

  ProcessSceneGraph(scene->mRootNode, scene);
}

void Doors::ProcessSceneGraph(aiNode* node, const aiScene* scene) {
  for (unsigned i = 0; i < node->mNumChildren; i++) {
    LoadPosition(node->mChildren[i], scene);
    ProcessSceneGraph(node->mChildren[i], scene);
  }
}

void Doors::LoadPosition(const aiNode* node, const aiScene* scene) {
  std::string name = node->mName.C_Str();

  glm::mat4 rotationCorrection = glm::rotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  glm::mat4 transform = Cast_aiMat4ToGlmMat4(node->mTransformation);
  glm::mat4 model = rotationCorrection * transform * glm::inverse(rotationCorrection);

  if (std::isdigit(name.back())) {
    int index = name.back() - '0';
    positions[index] = glm::vec3(model[3]);
  }
}

glm::mat4 Doors::Cast_aiMat4ToGlmMat4(const aiMatrix4x4& ai_matr) {
  return glm::mat4(
    ai_matr.a1, ai_matr.b1, ai_matr.c1, ai_matr.d1,
    ai_matr.a2, ai_matr.b2, ai_matr.c2, ai_matr.d2,
    ai_matr.a3, ai_matr.b3, ai_matr.c3, ai_matr.d3,
    ai_matr.a4, ai_matr.b4, ai_matr.c4, ai_matr.d4);
}