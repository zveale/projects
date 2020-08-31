#include <string>
#include "Scene.h"

SceneObject::SceneObject() : isQueried(false), isDrawn(false), queryId(0) {}

Scene::Scene() : GameObject(), drawElevatorShaft(true), queryProgram(0) {}

void Scene::Load() {
  order.resize(numScenes);

  for (int i = 0; i < numScenes; ++i) {
    scenes[i].lightScene.LoadModel("../files/asset/level/scene_" + std::to_string(i) + ".dae");
    scenes[i].occlusionScene.LoadModel("../files/asset/level/occlusion_scene_" + std::to_string(i) + ".dae");
    scenes[i].isQueried = false;
    scenes[i].isDrawn = true;
    glGenQueriesARB(1, &scenes[i].queryId);
    glBeginQueryARB(GL_SAMPLES_PASSED_ARB, scenes[i].queryId);
    glEndQueryARB(GL_SAMPLES_PASSED_ARB);
  }
  shadowScene.LoadModel("../files/asset/level/shadow_main.dae");

  for (int i = 0; i < numRooms; ++i) {
    const int charOffset = 97;
    std::string letter(1, (char)(i + charOffset));
    rooms[i].lightScene.LoadModel("../files/asset/level/room_" + letter + ".dae");
    rooms[i].occlusionScene.LoadModel("../files/asset/level/occlusion_room_" + letter + ".dae");
    rooms[i].isQueried = false;
    rooms[i].isDrawn = true;
    glGenQueriesARB(1, &rooms[i].queryId);
    glBeginQueryARB(GL_SAMPLES_PASSED_ARB, rooms[i].queryId);
    glEndQueryARB(GL_SAMPLES_PASSED_ARB);
    shadowRooms[i].LoadModel("../files/asset/level/room_" + letter + ".dae");
  }

  doorFrames.LoadModel("../files/asset/level/door_frames.dae");
  shadowDoorFrames.LoadModel("../files/asset/level/door_frames.dae");
}

void Scene::Update(float dt) {}

void Scene::Draw(ShaderProgram& shaderProgram, const glm::mat4& projectionMatrix, 
  const glm::mat4& viewMatrix, const int index) {
  CheckQueryObjects();

  if (!drawElevatorShaft)
    scenes[0].isDrawn = false;

  const glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
  const glm::mat4 viewProjectionMatrix = projectionMatrix * glm::mat4(glm::mat3(viewMatrix));
  const glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
  const glm::mat4 modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;

  shaderProgram.Use();
  shaderProgram.SetUniformMat4("modelMatrix", modelMatrix);
  shaderProgram.SetUniformMat3("normalMatrix", normalMatrix);
  shaderProgram.SetUniformMat4("viewMatrix", viewMatrix);
  shaderProgram.SetUniformMat4("projectionMatrix", projectionMatrix);
  shaderProgram.SetUniformMat4("viewProjectionMatrix", viewProjectionMatrix);
  shaderProgram.SetUniformMat4("modelViewProjectionMatrix", modelViewProjectionMatrix);
  shaderProgram.SetUniformBool("isAnimated", false);

  queryProgram->Use();
  queryProgram->SetUniformMat4("modelViewProjectionMatrix", modelViewProjectionMatrix);

  // draw scene parts
  const int sceneIndex = 0;
  if (index == sceneIndex) {
    shaderProgram.Use();
    doorFrames.Draw();

    for (auto i = order.begin(); i != order.end(); ++i) {
      glBeginQueryARB(GL_SAMPLES_PASSED_ARB, scenes[i->first].queryId);

      if (scenes[i->first].isDrawn) {
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
        glDepthMask(GL_TRUE);

        shaderProgram.Use();
        scenes[i->first].lightScene.Draw();

        glEndQueryARB(GL_SAMPLES_PASSED_ARB);
      }
      else {
        queryProgram->Use();

        glDisable(GL_CULL_FACE);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glDepthMask(GL_FALSE);

        scenes[i->first].occlusionScene.Draw();

        glEndQueryARB(GL_SAMPLES_PASSED_ARB);
        glDepthMask(GL_TRUE);
        glEnable(GL_CULL_FACE);
      }
    }
  }

  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glDepthMask(GL_TRUE);

  // draw rooms
  const int roomAIndex = 1, roomBIndex = 2, roomCIndex = 3, roomDIndex = 4;
  if (index == roomAIndex || index == roomBIndex || index == roomCIndex || index == roomDIndex) {
    glBeginQueryARB(GL_SAMPLES_PASSED_ARB, rooms[index - 1].queryId);

    if (rooms[index - 1].isDrawn) {
      glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
      glDepthMask(GL_TRUE);

      shaderProgram.Use();
      rooms[index - 1].lightScene.Draw();
  
      glEndQueryARB(GL_SAMPLES_PASSED_ARB);
    }
    else {
      glDisable(GL_CULL_FACE);
      glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
      glDepthMask(GL_FALSE);

      queryProgram->Use();
      rooms[index - 1].occlusionScene.Draw();
  
      glEndQueryARB(GL_SAMPLES_PASSED_ARB);
      glDepthMask(GL_TRUE);
      glEnable(GL_CULL_FACE);
    }
  }

  shaderProgram.Use();
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glDepthMask(GL_TRUE);
}

void Scene::DrawShadow(ShaderProgram& shaderProgram, const int index) {
  shaderProgram.SetUniformMat4("modelMatrix", modelMatrix);

  const int sceneIndex = 0;
  if (index == sceneIndex)
    shadowScene.Draw();

  const int roomAIndex = 1, roomBIndex = 2, roomCIndex = 3, roomDIndex = 4;
  if (index == roomAIndex || index == roomBIndex || index == roomCIndex || index == roomDIndex)
    if (rooms[index - 1].isDrawn)
      rooms[index - 1].lightScene.Draw();

  if (index == sceneIndex)
    shadowDoorFrames.Draw();
}

void Scene::Delete() {}

const unsigned Scene::NumElements() { return 1; }

void Scene::SendShaderData(ShaderProgram& shaderProgram, const int index) {}

void Scene::CheckQueryObjects() {
  for (int i = 0; i < numScenes; ++i) {
      GLuint sampleCount = 0;
      glGetQueryObjectuivARB(scenes[i].queryId, GL_QUERY_RESULT_ARB, &sampleCount);
      if (sampleCount > 0)
        scenes[i].isDrawn = true;
      else 
        scenes[i].isDrawn = false;

      order[i] = std::pair<int, unsigned>(i, sampleCount);
  }

  std::sort(order.begin(), order.end(), Compare());

  for (int i = 0; i < numRooms; ++i) {
      GLuint sampleCount = 0;
      glGetQueryObjectuivARB(rooms[i].queryId, GL_QUERY_RESULT_ARB, &sampleCount);
      if (sampleCount > 0)
        rooms[i].isDrawn = true;
      else
        rooms[i].isDrawn = false;
  }
}

void Scene::AttachQueryProgram(ShaderProgram* queryProgram) {
  this->queryProgram = queryProgram;
}

void Scene::NoElevatorShaft() {
  drawElevatorShaft = false;
}

void Scene::DrawInstance(ShaderProgram& shaderProgram, const glm::mat4& projectionMatrix,
  const glm::mat4& viewMatrix, const int index) {
  const glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
  const glm::mat4 viewProjectionMatrix = projectionMatrix * glm::mat4(glm::mat3(viewMatrix));
  const glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
  const glm::mat4 modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;

  shaderProgram.Use();
  shaderProgram.SetUniformMat4("modelMatrix", modelMatrix);
  shaderProgram.SetUniformMat3("normalMatrix", normalMatrix);
  shaderProgram.SetUniformMat4("viewMatrix", viewMatrix);
  shaderProgram.SetUniformMat4("projectionMatrix", projectionMatrix);
  shaderProgram.SetUniformMat4("viewProjectionMatrix", viewProjectionMatrix);
  shaderProgram.SetUniformMat4("modelViewProjectionMatrix", modelViewProjectionMatrix);
  shaderProgram.SetUniformBool("isAnimated", false);

  queryProgram->Use();
  queryProgram->SetUniformMat4("modelViewProjectionMatrix", modelViewProjectionMatrix);

  // draw scene
  const int sceneIndex = 0;
  if (index == sceneIndex) {
    shaderProgram.Use();
    doorFrames.Draw();

    for (auto i = order.begin(); i != order.end(); ++i) {
      glBeginQueryARB(GL_SAMPLES_PASSED_ARB, scenes[i->first].queryId);

      if (scenes[i->first].isDrawn) {
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
        glDepthMask(GL_TRUE);

        shaderProgram.Use();
        scenes[i->first].lightScene.Draw();

        glEndQueryARB(GL_SAMPLES_PASSED_ARB);
      }
      else {
        queryProgram->Use();

        glDisable(GL_CULL_FACE);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glDepthMask(GL_FALSE);

        scenes[i->first].occlusionScene.Draw();


        glEndQueryARB(GL_SAMPLES_PASSED_ARB);
        glDepthMask(GL_TRUE);
        glEnable(GL_CULL_FACE);
      }
    }
  }

  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glDepthMask(GL_TRUE);

  // draw rooms
  const int roomAIndex = 1, roomBIndex = 2, roomCIndex = 3, roomDIndex = 4;
  if (index == roomAIndex || index == roomBIndex || index == roomCIndex || index == roomDIndex) {
    glBeginQueryARB(GL_SAMPLES_PASSED_ARB, rooms[index - 1].queryId);

    if (rooms[index - 1].isDrawn) {
      glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
      glDepthMask(GL_TRUE);

      shaderProgram.Use();
      rooms[index - 1].lightScene.Draw();

      glEndQueryARB(GL_SAMPLES_PASSED_ARB);
    }
    else {
      glDisable(GL_CULL_FACE);
      glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
      glDepthMask(GL_FALSE);

      queryProgram->Use();
      rooms[index - 1].occlusionScene.Draw();

      glEndQueryARB(GL_SAMPLES_PASSED_ARB);
      glDepthMask(GL_TRUE);
      glEnable(GL_CULL_FACE);
    }
  }

  shaderProgram.Use();
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glDepthMask(GL_TRUE);

  if (index == sceneIndex)
    doorFrames.Draw();
}

void Scene::DrawInstanceLightProbe(ShaderProgram& shaderProgram, const glm::mat4& projectionMatrix, 
  const glm::mat4& viewMatrix, const int index) {
  const glm::mat4 viewProjectionMatrix = projectionMatrix * glm::mat4(viewMatrix);
  const glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));

  shaderProgram.SetUniformMat4("modelMatrix", modelMatrix);
  shaderProgram.SetUniformMat3("normalMatrix", normalMatrix);
  shaderProgram.SetUniformMat4("viewProjectionMatrix", viewProjectionMatrix);

  const int sceneIndex = 0;
  if (index == sceneIndex) {
      doorFrames.Draw();

      for (int i = 0; i < numScenes; ++i)
        scenes[i].lightScene.Draw();
  }

  const int roomAIndex = 1, roomBIndex = 2, roomCIndex = 3, roomDIndex = 4;
  if (index == roomAIndex || index == roomBIndex || index == roomCIndex || index == roomDIndex) {
    if (rooms[index - 1].isDrawn)
      rooms[index - 1].lightScene.Draw();
  }
}
