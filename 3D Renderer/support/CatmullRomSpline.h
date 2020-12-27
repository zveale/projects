// Game Programming Algorithms and Techniques: A Platform-Agnostic Approach

#ifndef CATMULLROMSPLINE_H
#define CATMULLROMSPLINE_H

#include <vector>
#include <glm\vec3.hpp>

class CatmullRomSpline {
public:
  void LoadPositions(const std::vector<glm::vec3>& points);
  glm::vec3 FindCurrentPosition(float time);
  void IncrementIndex();
  void SetIndex(int index);

private:
  std::vector<glm::vec3> points;
  glm::vec3 position;
  int index;

  glm::vec3 CatmullRomSplineEquation(const glm::vec3& P0, const glm::vec3& P1, const glm::vec3& P2, const glm::vec3& P3, float time);
};

#endif // !CATMULLROMSPLINE_H
