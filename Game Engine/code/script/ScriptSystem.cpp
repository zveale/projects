#include <filesystem>
#include <string>
#include "ScriptSystem.h"
#include "Global.h"
#include "Enemy.h"

ScriptSystem::ScriptSystem() {
  py::module sys = py::module::import("sys");
  const char* pathToScripts = "C:\\Users\\puraw\\Source\\Repos\\intercom-626\\Intercom 626\\files\\scripts";
  sys.attr("path").attr("insert")(0, pathToScripts);
}

void ScriptSystem::StartUp() {
  std::string pathToDirectory = "../files/scripts";
  for (const std::filesystem::path& file : std::filesystem::directory_iterator(pathToDirectory)) {
    std::string path = file.string();

    if (path.find("pycache") == std::string::npos) {
      // converts "../files/scripts\modulename.py" into "modulename"
      std::size_t position = path.rfind("\\") + 1; 
      std::size_t length = path.substr(position).size() - 3;
      std::string moduleName = path.substr(position, length);

      py::module module = py::module::import(moduleName.c_str());
      modules.emplace_back(module);

      moduleMap[moduleName] = modules.size() - 1;
    }
  }
}

void ScriptSystem::Update(float dt) {
  if (GLOBAL::RELOAD_SCRIPTS) {
    for (py::module& module : modules)
      module.reload();

    GLOBAL::RELOAD_SCRIPTS = false;
  }

  Enemy* enemy = (Enemy*)(gameObjects[static_cast<int>(ID::ENEMY)]);
  int moduleIndex = moduleMap["EnemyScript"];
  modules[moduleIndex].attr("UpdateCommand")(enemy, dt);
}

void ScriptSystem::ShutDown() {}

void ScriptSystem::AttachGameObjects(GameObject** gameObjects) {
  this->gameObjects = gameObjects;
}
