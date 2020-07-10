#include "Game.h"

void Game::StartUp() {
  renderSystem.StartUp();
  inputSystem.StartUp();
  logicSystem.StartUp();
  physicsSystem.StartUp();
  scriptSystem.StartUp();

  playerCamera.Load();
  cinematicCamera.Load();
  player.Load();
  enemy.Load();
  scene.Load();
  lights.Load();
  switches.Load();
  doors.Load();
  windows.Load();
  skybox.Load();
  items.Load();
  elevator.Load();

  gameObjects[static_cast<int>(ID::PLAYER_CAMERA)] = &playerCamera;
  gameObjects[static_cast<int>(ID::CINEMATIC_CAMERA)] = &cinematicCamera;
  gameObjects[static_cast<int>(ID::PLAYER)] = &player;
  gameObjects[static_cast<int>(ID::ENEMY)] = &enemy;
  gameObjects[static_cast<int>(ID::SCENE)] = &scene;
  gameObjects[static_cast<int>(ID::LIGHTS)] = &lights;
  gameObjects[static_cast<int>(ID::SWITCHES)] = &switches;
  gameObjects[static_cast<int>(ID::DOORS)] = &doors;
  gameObjects[static_cast<int>(ID::WINDOW)] = &windows;
  gameObjects[static_cast<int>(ID::SKYBOX)] = &skybox;
  gameObjects[static_cast<int>(ID::ITEMS)] = &items;
  gameObjects[static_cast<int>(ID::ELEVATOR)] = &elevator;

  renderSystem.AttachGameObjects(gameObjects);
  logicSystem.AttachGameObjects(gameObjects);
  logicSystem.AttachInput(&inputSystem);
  logicSystem.AttachCollisionData(&physicsSystem);
  physicsSystem.AttachPlayer(&player);
  physicsSystem.AttachDoors(&doors);
  scriptSystem.AttachGameObjects(gameObjects);

  playerCamera.AttachInput(&inputSystem);
  cinematicCamera.SetTargetPosition(&player.position); //TODO
  player.AttachCamera(playerCamera);
  player.AttachLight(lights.GetPlayerLight());
  enemy.AttachPlayer(player);

  physicsSystem.LateStartup();
  renderSystem.LateStartUp();

  GLOBAL::GAME_STATE = GLOBAL::GAMESTATE::ELEVATOR_START;
}

void Game::Run() {
  float lastTime = SDL_GetTicks() / 1000.0f;

  while (!inputSystem.Quit()) {
    float currentTime = SDL_GetTicks() / 1000.0f;
    float dt = currentTime - lastTime;
    lastTime = currentTime;

    GLOBAL::GAME_TIME += dt;

    inputSystem.Process();
    scriptSystem.Update(dt);
    logicSystem.Update(dt);
    // physicsSystem.Update();
    logicSystem.PostUpdate();
    renderSystem.Render();
  }
}

void Game::ShutDown() {
  elevator.Delete();
  items.Delete();
  skybox.Delete();
  windows.Delete();
  switches.Delete();
  doors.Delete();
  lights.Delete();
  scene.Delete();
  enemy.Delete();
  player.Delete();
  cinematicCamera.Delete();
  playerCamera.Delete();

  scriptSystem.ShutDown();
  physicsSystem.ShutDown();
  logicSystem.ShutDown();
  inputSystem.ShutDown();
  renderSystem.ShutDown();
}
