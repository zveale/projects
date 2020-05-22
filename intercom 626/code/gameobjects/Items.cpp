#include <string>
#include <cctype>
#include "Items.h"
#include "assimp\Importer.hpp"
#include "assimp\scene.h"
#include "GameObject.h"
#include "StaticModel.h"
#include "glm\vec3.hpp"
#include "glm\mat4x4.hpp"
#include "glm\mat3x3.hpp"
#include "glm\gtx\transform.hpp"

Items::Items() : GameObject() {
  for (int i = 0; i < numItems; ++i) {
    sceneMatrix[i] = glm::mat4(0.0f);
    inventoryMatrix[i] = glm::mat4(0.0f);
    slotMatrix[i] = glm::mat4(0.0f);
  }
}

void Items::Load() {
  models[0].LoadModel("../files/asset/items/0_item.dae");
  models[1].LoadModel("../files/asset/items/1_item.dae");
  models[2].LoadModel("../files/asset/items/2_item.dae");
  models[3].LoadModel("../files/asset/items/3_item.dae");

  for (int i = 0; i < numItems; ++i)
    staticModels[i] = &models[i];

  ids[0] = "0_item";
  ids[1] = "1_item";
  ids[2] = "2_item";
  ids[3] = "3_item";

  LoadMatrices("../files/asset/items/item_matrices.dae");

  state[0] = STATE::SCENE;
  state[1] = STATE::SCENE;
  state[2] = STATE::SCENE;
  state[3] = STATE::SCENE;

  currentMatrix[0] = &sceneMatrix[0];
  currentMatrix[1] = &sceneMatrix[1];
  currentMatrix[2] = &sceneMatrix[2];
  currentMatrix[3] = &sceneMatrix[3];
}

void Items::Update(float dt) {  }

void Items::Draw(ShaderProgram& shaderProgram, int index) {
  models[index].Draw(shaderProgram);
}

void Items::Delete() {}

void Items::SendShaderData(ShaderProgram& shaderProgram, const int index) {
  modelMatrix = *currentMatrix[index];
}

const unsigned Items::NumElements() { return numItems; }

/*
Pickup item at index. If it is in scene move to invetory(render item underneath scene), if the
item is in the inventory move it into its final scene location.
*/
void Items::PickupItem(int index) {
  if (state[index] == STATE::SCENE && numItemsInInventory != MAX_INVENTORY_ITEMS) {
    state[index] = STATE::INVENTORY;
    currentMatrix[index] = &inventoryMatrix[index];
    ++numItemsInInventory;

    if (numItemsInInventory == MAX_INVENTORY_ITEMS)
      lastItemIndex = index;
  }
  else if (state[index] == STATE::SCENE && numItemsInInventory == MAX_INVENTORY_ITEMS) {
    state[index] = STATE::INVENTORY;
    currentMatrix[index] = &inventoryMatrix[index];

    state[lastItemIndex] = STATE::SCENE;
    sceneMatrix[lastItemIndex] = sceneMatrix[index];
    currentMatrix[lastItemIndex] = &sceneMatrix[lastItemIndex];

    lastItemIndex = index;
  }
  else if (state[index] == STATE::INVENTORY) {
    state[index] = STATE::SLOT;
    currentMatrix[index] = &slotMatrix[index];
    --numItemsInInventory;
  }
}

StaticModel** Items::GetModels() { return staticModels; }
const char** Items::GetIds() { return ids; }
glm::mat4** Items::GetMatrices() { return currentMatrix; }

void Items::LoadMatrices(std::string path) {
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(path, NULL);

  if (!scene || !scene->mRootNode) {
    printf("Error loading shadows!\n Error message: %s\n", importer.GetErrorString());
    return;
  }

  ProcessSceneGraph(scene->mRootNode, scene);
}

void Items::ProcessSceneGraph(aiNode* node, const aiScene* scene) {
  for (unsigned i = 0; i < node->mNumChildren; i++) {
    CreateItem(node->mChildren[i], scene);
    ProcessSceneGraph(node->mChildren[i], scene);
  }
}

void Items::CreateItem(const aiNode* node, const aiScene* scene) {
  std::string name = node->mName.C_Str();

  glm::mat4 rotationCorrection = glm::rotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  glm::mat4 transform = Cast_aiMat4ToGlmMat4(node->mTransformation);
  glm::mat4 model = rotationCorrection * transform * glm::inverse(rotationCorrection);

  if (std::isdigit(name[0])) {
    int index = name[0] - '0';

    if (name.find("scene") != std::string::npos)
      sceneMatrix[index] = model;
    else if (name.find("inventory") != std::string::npos)
      inventoryMatrix[index] = model;
    else if (name.find("slot") != std::string::npos)
      slotMatrix[index] = model;
  }
}

glm::mat4 Items::Cast_aiMat4ToGlmMat4(const aiMatrix4x4& ai_matr) {
  return glm::mat4(
    ai_matr.a1, ai_matr.b1, ai_matr.c1, ai_matr.d1,
    ai_matr.a2, ai_matr.b2, ai_matr.c2, ai_matr.d2,
    ai_matr.a3, ai_matr.b3, ai_matr.c3, ai_matr.d3,
    ai_matr.a4, ai_matr.b4, ai_matr.c4, ai_matr.d4);
}
glm::vec3 Items::Cast_aiVec3DToGlmVec3(const aiVector3D& aiVec) {
  return glm::vec3(aiVec.x, aiVec.y, aiVec.z);
}