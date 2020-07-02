#ifndef GAME_H
#define GAME_H

#include "RenderSystem.h"
#include "InputSystem.h"
#include "LogicSystem.h"
#include "PhysicsSystem.h"
#include "FirstPersonCamera.h"
#include "CinematicCamera.h"
#include "GameObject.h"
#include "Player.h"
#include "Enemy.h"
#include "Scene.h"
#include "Lights.h"
#include "Doors.h"
#include "Switches.h"
#include "Window.h"
#include "Skybox.h"
#include "Items.h"
#include "Elevator.h"

class Game {
public:
  /*
  Intialize all systems and gameobjects.
  */
  void StartUp();

  /*
  Gameloop.
  */
  void Run();

  /*
  Destroy all systems and gameobjects.
  */
  void ShutDown();

private:
  RenderSystem renderSystem;
  InputSystem inputSystem;
  LogicSystem logicSystem;
  PhysicsSystem physicsSystem;

  FirstPersonCamera playerCamera;
  CinematicCamera cinematicCamera;

  GameObject* gameObjects[ID::NUM_GAME_OBJECTS];
  Player player;
  Enemy enemy;
  Scene scene;
  Lights lights;
  Doors doors;
  Switches switches;
  Window windows;
  Skybox skybox;
  Items items;
  Elevator elevator;
};

#endif // !GAME_H