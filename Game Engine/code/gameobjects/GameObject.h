#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "ShaderProgram.h"
#include "glm\mat4x4.hpp"

enum class ID { PLAYER_CAMERA, CINEMATIC_CAMERA, PLAYER, ENEMY, SCENE, LIGHTS, 
  DOORS, SWITCHES, WINDOW, SKYBOX, ITEMS, ELEVATOR, NUM_GAME_OBJECTS };

class GameObject {
public:
  /*
  Initialize members.
  */
  GameObject() { modelMatrix = glm::mat4(1.0f); }

  /*
  Load models and set default values that could not be set in blender.
  */
  virtual void Load() = 0;

  /*
  This update function will be called in the logic system. Will update state of gameobject.
  */
  virtual void Update(float dt = 0.0f) = 0;

  /*
  **This is depreciated. The scene manager now calls the draw function for game object models directly**
  Calls the draw function for the models belonging to the gameobject. 
  */
  virtual void Draw(ShaderProgram& shaderProgram, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const int index = 0) = 0;

  /*
  Remove any allocated memory for members.
  */
  virtual void Delete() = 0;

  /*
  Send any state changes and data to the currently attached shader program.
  */
  virtual void SendShaderData(ShaderProgram& shaderProgram, const int index = 0) = 0;

  /*
  Some gameobjects have several models. This will be the number of models.
  */
  virtual const unsigned NumElements() = 0;

  /*
  Rarely changed from identity matrix.
  */
  const glm::mat4& GetModelMatrix() const { return modelMatrix; }

protected:
  glm::mat4 modelMatrix;
};

#endif // !GAMEOBJECT_H
