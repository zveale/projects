#ifndef ITEMS_H
#define ITEMS_H

#include <string>
#include <cctype>
#include "assimp\Importer.hpp"
#include "assimp\scene.h"
#include "GameObject.h"
#include "StaticModel.h"
#include "glm\vec3.hpp"
#include "glm\mat4x4.hpp"
#include "glm\mat3x3.hpp"
#include "glm\gtx\transform.hpp"

class Items: public GameObject {
public:
  Items();
  void Load();
  void Update(float dt = 0.0f);
  void Draw(ShaderProgram& shaderProgram, int index = 0);
  void Delete();
  void SendShaderData(ShaderProgram& shaderProgram, const int index = 0);
  const unsigned NumElements();

  /*
  Pickup item at index. If it is in scene move to invetory(render item underneath scene), if the
  item is in the inventory move it into its final scene location.
  */
  void PickupItem(int index);

  StaticModel** GetModels();
  const char** GetIds();
  glm::mat4** GetMatrices();

private:
  const static int numItems = 4;
  StaticModel* staticModels[numItems];
  StaticModel models[numItems];
  const char* ids[numItems];
  glm::mat4 sceneMatrix[numItems];
  glm::mat4 inventoryMatrix[numItems];
  glm::mat4 slotMatrix[numItems];
  glm::mat4* currentMatrix[numItems];

  enum STATE { SCENE, INVENTORY, SLOT, NUM_STATES } state[numItems];

  const int MAX_INVENTORY_ITEMS = 3;
  int numItemsInInventory = 0;
  int lastItemIndex = -1;

  /*
  Load blender file with matrix locations of items.
  */
  void LoadMatrices(std::string path);
  void ProcessSceneGraph(aiNode* node, const aiScene* scene);
  void CreateItem(const aiNode* node, const aiScene* scene);

  glm::mat4 Cast_aiMat4ToGlmMat4(const aiMatrix4x4& ai_matr);
  glm::vec3 Cast_aiVec3DToGlmVec3(const aiVector3D& aiVec);
};

#endif // !ITEMS_H
