#include "CatmullRomSpline.h"

void CatmullRomSpline::LoadPositions(const std::vector<glm::vec3>& points) {
  this->points = points;

  if (!points.empty())
    position = points.front();
  index = 0;
}

glm::vec3 CatmullRomSpline::FindCurrentPosition(float time) {
  if (index - 1 > 0 && index + 2 < points.size()) {
    glm::vec3 P0(points[index - 1]);
    glm::vec3 P1(points[index]);
    glm::vec3 P2(points[index + 1]);
    glm::vec3 P3(points[index + 2]);

    position = CatmullRomSplineEquation(P0, P1, P2, P3, time);
  }

  return position;
}

void CatmullRomSpline::IncrementIndex() { ++index; }

void CatmullRomSpline::SetIndex(int index) { this->index = index; }

glm::vec3 CatmullRomSpline::CatmullRomSplineEquation(const glm::vec3& P0, const glm::vec3& P1, const glm::vec3& P2,  const glm::vec3& P3, float time) {
  return 0.5f * ((2.0f * P1) + (-P0 + P2) * time + (2.0f * P0 - 5.0f * P1 + 4.0f * P2 - P3) *
    time * time + (-P0 + 3.0f * P1 - 3.0f * P2 + P3) * time * time * time);
}