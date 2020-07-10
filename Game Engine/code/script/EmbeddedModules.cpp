// Embedding Python in a C++ project with Visual Studio: https://devblogs.microsoft.com/python/embedding-python-in-a-cpp-project-with-visual-studio/

#pragma once

#include <pybind11/embed.h>
namespace py = pybind11;
#include "Enemy.h"

PYBIND11_EMBEDDED_MODULE(Commands, m) {
  py::class_<Enemy, std::unique_ptr<Enemy, py::nodelete>>(m, "Enemy")
    .def("StandToCrawl", &Enemy::StandToCrawl)
    .def("CrawlToStand", &Enemy::CrawlToStand)
    .def("MoveForward", &Enemy::MoveForward)
    .def("TurnRight180", &Enemy::TurnRight180)
    .def("PathfindToPlayer", &Enemy::PathfindToPlayer)
    .def_readwrite("currentDuration", &Enemy::currentDuration)
    .def_readwrite("commandIndex", &Enemy::commandIndex);
}
