#include "Game.h"

void Game::StartUp() {
  renderSystem.StartUp();
  inputSystem.StartUp();
  logicSystem.StartUp();
  physicsSystem.StartUp();

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

  gameObjects[ID::PLAYER_CAMERA] = &playerCamera;
  gameObjects[ID::CINEMATIC_CAMERA] = &cinematicCamera;
  gameObjects[ID::PLAYER] = &player;
  gameObjects[ID::ENEMY] = &enemy;
  gameObjects[ID::SCENE] = &scene;
  gameObjects[ID::LIGHTS] = &lights;
  gameObjects[ID::SWITCHES] = &switches;
  gameObjects[ID::DOORS] = &doors;
  gameObjects[ID::WINDOWS] = &windows;
  gameObjects[ID::SKYBOX] = &skybox;
  gameObjects[ID::ITEMS] = &items;
  gameObjects[ID::ELEVATOR] = &elevator;

  renderSystem.AttachGameObjects(gameObjects);
  logicSystem.AttachGameObjects(gameObjects);
  logicSystem.AttachInput(&inputSystem);
  logicSystem.AttachCollisionData(&physicsSystem);
  physicsSystem.AttachPlayer(&player);
  physicsSystem.AttachDoors(&doors);

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

  physicsSystem.ShutDown();
  logicSystem.ShutDown();
  inputSystem.ShutDown();
  renderSystem.ShutDown();
}
