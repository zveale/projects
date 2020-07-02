/*
Chapter 29. Efficient Occlusion Culling: https://developer.download.nvidia.com/books/HTML/gpugems/gpugems_ch29.html
*/

#ifndef SCENE_H
#define SCENE_H

#include "GameObject.h"
#include "StaticModel.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include <map>
#include <vector>
#include <algorithm>

struct SceneObject {
  StaticModel lightScene;
  StaticModel occlusionScene;
  GLuint queryId;
  bool isQueried;
  bool isDrawn;

  SceneObject();
};

class Scene : public GameObject {
public:
  Scene();

  /*
  Load scene parts and intialize query results. Scene parts can be a detailed scene or a low poly shadow scene.
  */
  void Load();

  void Update(float dt = 0.0f);

  /*
  Check query result of each scene part from last frame. If scene part was rendered render it again else render
  the its bounding box. Index is used to pick specific scene parts or rooms.
  */
  void Draw(ShaderProgram& shaderProgram, const glm::mat4& projectionMatrix, 
    const glm::mat4& viewMatrix, const int index = 0);
  void DrawInstance(ShaderProgram& shaderProgram, const glm::mat4& projectionMatrix,
    const glm::mat4& viewMatrix, const int index);
  void DrawInstanceLightProbe(ShaderProgram& shaderProgram, const glm::mat4& projectionMatrix,
    const glm::mat4& viewMatrix, const int index);

  /*
  Draw the low poly model.
  */
  void DrawShadow(ShaderProgram& shaderProgram, const int index);

  void Delete();
  const unsigned NumElements();
  void SendShaderData(ShaderProgram& shaderProgram, const int index = 0);

  void AttachQueryProgram(ShaderProgram* queryProgram);
  void NoElevatorShaft();

private:
  const static unsigned numScenes = 22;
  SceneObject scenes[numScenes];
  StaticModel shadowScene;
  bool drawElevatorShaft;

  const static unsigned numRooms = 4;
  SceneObject rooms[numRooms];
  StaticModel shadowRooms[numRooms];

  StaticModel doorFrames;
  StaticModel shadowDoorFrames;

  // Used to sort the scene parts with the sample count in descending order. This allows the scene parts
  // that take up most of the screen to occluded scene parts with smaller sample counts.
  std::vector<std::pair<int, unsigned>> order;
  struct Compare {
    bool operator()(const std::pair<int, unsigned>& lhs, const std::pair<int, unsigned>& rhs) {
      return lhs.second > rhs.second;
    }
  };
  ShaderProgram* queryProgram;
  void CheckQueryObjects();
};

#endif // !SCENE_H
