// Embedding Python in a C++ project with Visual Studio: https://devblogs.microsoft.com/python/embedding-python-in-a-cpp-project-with-visual-studio/
// pybind11 — Seamless operability between C++11 and Python: https://pybind11.readthedocs.io/en/stable/

#include <vector>
#include <unordered_map>
#include <string>
#include <pybind11/embed.h>
namespace py = pybind11;
#include <GameObject.h>

#ifndef SCRIPTSYSTEM_H
#define SCRIPTSYSTEM_H

class ScriptSystem {
public:
  ScriptSystem();

  /*
  Load and import all py::module located in folder "../files/scripts"
  */
  void StartUp();

  /*
  If changes to module were made and reload-key(F1) pressed then reload module. Updates the enemy script.
  */
  void Update(float dt = 0.0f);

  void ShutDown();

  void AttachGameObjects(GameObject** gameObjects);

private:
  py::scoped_interpreter guard{};

  std::unordered_map<std::string, int> moduleMap;
  std::vector<py::module> modules;

  GameObject** gameObjects;
};

#endif SCRIPTSYSTEM_H

