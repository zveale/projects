// Embedding Python in a C++ project with Visual Studio: https://devblogs.microsoft.com/python/embedding-python-in-a-cpp-project-with-visual-studio/

#pragma once

#include <pybind11/embed.h>
namespace py = pybind11;
#include "Enemy.h"

PYBIND11_EMBEDDED_MODULE(Commands, m) {
  py::class_<Enemy, std::unique_ptr<Enemy, py::nodelete>>(m, "Enemy")
    .def("stand_to_crawl", &Enemy::StandToCrawl)
    .def("crawl_to_stand", &Enemy::CrawlToStand)
    .def("move_forward", &Enemy::MoveForward)
    .def("turn_right_180", &Enemy::TurnRight180)
    .def("pathfind_to_player", &Enemy::PathfindToPlayer)
    .def_readwrite("currentDuration", &Enemy::currentDuration)
    .def_readwrite("commandIndex", &Enemy::commandIndex);
}
