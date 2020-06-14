#include "Pathfinding.h"

#include <fstream>
#include <string>
#include <iterator>
#include <unordered_set>
#include <cmath>
#include "glm/geometric.hpp"

Node::Node() : parent(nullptr), gCost(0), hCost(0), isWalkable(false), gridIndex(glm::vec2(0, 0)) {}

Node::Node(int isWalkable, const glm::vec2& gridIndex) : 
  parent(nullptr), gCost(0), hCost(0), isWalkable(isWalkable), gridIndex(gridIndex) {}

int Node::FCost() const { return gCost + hCost; }

void Pathfinding::Load() {
  std::ifstream file("../files/asset/pathfinding/walkableNodes.txt", std::fstream::in);
  std::vector<std::string> walkableNodes;

  if (file.is_open()) {
    std::string buffer;

    while (!file.eof()) {
      file >> buffer;
      walkableNodes.push_back(buffer);
    }
  }
  file.close();

  // Indices are swapped to easy nodeGrid indexing later.
  for (int i = 0; i < walkableNodes.size(); ++i) {
    std::vector<Node> nodes;
    for (int j = 0; j < walkableNodes[i].size(); ++j) {
      int binary = (walkableNodes[j][i] - '0');
      nodes.emplace_back((bool)binary, glm::vec2((int)i, (int)j));
    }

    nodeGrid.push_back(nodes);
  }

  gridWidth = (int)nodeGrid.at(0).size();
  gridHeight = (int)nodeGrid.size();
}

void Pathfinding::FindPath(const glm::vec3& startPosition, const glm::vec3& targetPosition) {
  Node* start = FindNodeInGrid(startPosition);
  Node* target = FindNodeInGrid(targetPosition);

  std::unordered_set<Node*> open;
  std::unordered_set<Node*> closed;

  open.insert(start);
  while (!open.empty()) {
    Node* current = *open.begin();

    // Find node in openSet with lowest cost and assign to currentNode.
    for (auto it = ++open.begin(); it != open.end(); ++it) {
      if ((*it)->FCost() < current->FCost())
        current = (*it);
      else if ((*it)->FCost() == current->FCost())
        if ((*it)->hCost < current->hCost)
          current = (*it);
    }

    open.erase(current);
    closed.insert(current);

    // Path has been found. Return list of positions.
    if (current == target) {
      CalculatePathPostions(startPosition, targetPosition);
      return;
    }

    // Find the cost of each neighbor.
    for (Node* neighbor : FindNeighboringNodes(current)) {
      if (!neighbor->isWalkable || closed.find(neighbor) != closed.end())
        continue;

      int neighborCost = current->gCost + FindCostBetweenNodes(current, neighbor);
      if (neighborCost < neighbor->gCost || open.find(neighbor) == open.end()) {
        neighbor->gCost = neighborCost;
        neighbor->hCost = FindCostBetweenNodes(neighbor, target);
        neighbor->parent = current;

        if (open.find(neighbor) == open.end())
          open.insert(neighbor);
      }
    }
  }
}

glm::vec3 Pathfinding::FirstPosition() {
  if (!positionalList.empty()) {
    glm::vec3 nextPosition = positionalList.front();
    positionalList.pop_front();

    return nextPosition;
  }

  return glm::vec3(0.0f);
}

glm::vec3 Pathfinding::SecondPostion() {
  if (!positionalList.empty()) {
    auto it = ++positionalList.begin();
    if (it != positionalList.end())
      return *it;
    else
      return *--it;
  }

  return glm::vec3(0.0f);
}

bool Pathfinding::NoPath() { return positionalList.empty(); }

std::vector<glm::vec3> Pathfinding::GetList() const {
  return std::vector<glm::vec3>(positionalList.begin(), positionalList.end());
}

void Pathfinding::AddPositionToList(const glm::vec3& position) { positionalList.push_front(position); }

void Pathfinding::SmoothPath() {
  if (positionalList.size() > 2) {
    auto index = positionalList.begin();
    glm::vec3 start = *index;
    glm::vec3 target = *++index;

    auto end = --positionalList.end();
    for (auto it = ++index; it != end; ++it) {
      target = *it;

      if (IsPositionWalkable(start, target))
        positionalList.erase(it);
      else
        start = target;
    }
  }
}

bool Pathfinding::IsPositionWalkable(const glm::vec3& position, const glm::vec3& target) {

  float sideScale = 0.45f;
  const int max_samples = (int)glm::distance(position, target);
  for (int i = max_samples; i >= 1; --i) {
    float frontScale = i * 0.2f;
    glm::vec3 forwardDirection = glm::normalize(target - position) * frontScale;
    glm::vec3 forwardPosition = position + forwardDirection;

    glm::vec3 leftDirection = glm::normalize(glm::cross(glm::normalize(forwardPosition - position), glm::vec3(0.0f, 1.0f, 0.f))) * sideScale;
    glm::vec3 leftPosition = forwardPosition + leftDirection;

    glm::vec3 rightDirection = -leftDirection;
    glm::vec3 rightPosition = forwardPosition + rightDirection;

    if (!(IsPositionInGridWalkable(forwardPosition) && IsPositionInGridWalkable(leftPosition) &&
      IsPositionInGridWalkable(rightPosition)))
      return false;
  }

  return true;
}

bool Pathfinding::IsPositionInGridWalkable(const glm::vec3& position) {
  glm::vec2 nodeIndex = glm::vec2(
    (int)(position.x * 2) + gridWidth / 2,
    (int)(position.z * 2) + gridHeight / 2);

  if (nodeIndex.x < 0 || nodeIndex.y < 0 || nodeIndex.x > nodeGrid.size() - 1 ||
    nodeIndex.y > nodeGrid[(unsigned)nodeIndex.x].size() - 1)
    return false;

  if (nodeGrid[(unsigned)nodeIndex.x][(unsigned)nodeIndex.y].isWalkable)
    return true;

  return false;
}

Node* Pathfinding::FindNodeInGrid(const glm::vec3& position) {
  glm::vec2 nodeIndex = glm::vec2(
    (int)(position.x * 2) + gridWidth / 2,
    (int)(position.z * 2) + gridHeight / 2);

  if (nodeIndex.x < 0 || nodeIndex.y < 0 || nodeIndex.x > nodeGrid.size() - 1 ||
    nodeIndex.y > nodeGrid[(unsigned)nodeIndex.x].size() - 1)
    nodeIndex = glm::vec2(0, 0);

  if (nodeGrid[(unsigned)nodeIndex.x][(unsigned)nodeIndex.y].isWalkable)
    return &nodeGrid[(unsigned)nodeIndex.x][(unsigned)nodeIndex.y];

  for (int i = -4; i <= 4; ++i)
    for (int j = -4; j <= 4; ++j)
      if (!(i == 0 && j == 0)) {
        glm::vec2 neighborIndex = glm::vec2(nodeIndex.x + i, nodeIndex.y + j);

        if (neighborIndex.x >= 0 && neighborIndex.x < gridWidth &&
          neighborIndex.y >= 0 && neighborIndex.y < gridHeight)
          if (nodeGrid[(unsigned)neighborIndex.x][(unsigned)neighborIndex.y].isWalkable)
            return &nodeGrid[(unsigned)neighborIndex.x][(unsigned)neighborIndex.y];
      }

  return &nodeGrid[0][0];
}

std::list<Node*> Pathfinding::FindNeighboringNodes(Node* currentNode) {
  std::list<Node*> neighbors;

  for (int i = -1; i <= 1; ++i)
    for (int j = -1; j <= 1; ++j)
      if (!(i == 0 && j == 0)) {
        glm::vec2 neighborIndex = glm::vec2(currentNode->gridIndex.x + i,
          currentNode->gridIndex.y + j);

        if (neighborIndex.x >= 0 && neighborIndex.x < gridWidth &&
          neighborIndex.y >= 0 && neighborIndex.y < gridHeight)
          neighbors.push_back(&nodeGrid[(unsigned)neighborIndex.x][(unsigned)neighborIndex.y]);
      }

  return neighbors;
}

int Pathfinding::FindCostBetweenNodes(Node* nodeA, Node* nodeB) {
  glm::vec2 distance(std::abs(nodeA->gridIndex.x - nodeB->gridIndex.x),
    std::abs(nodeA->gridIndex.y - nodeB->gridIndex.y));

  if (distance.x > distance.y)
    return (int)(DIAGONAL_COST * distance.y + NON_DIAGONAL_COST * (distance.x - distance.y));

  return (int)(DIAGONAL_COST * distance.x + NON_DIAGONAL_COST * (distance.y - distance.x));
}

void Pathfinding::CalculatePathPostions(const glm::vec3& startPosition, const glm::vec3& targetPosition) {
  Node* startNode = FindNodeInGrid(startPosition);
  Node* targetNode = FindNodeInGrid(targetPosition);
  Node* currentNode = targetNode;

  positionalList.clear();

  while (currentNode != startNode) {
    positionalList.emplace_front(glm::vec3(
      (currentNode->gridIndex.x - gridWidth / 2) / 2.0f,
      0.0f,
      (currentNode->gridIndex.y - gridHeight / 2) / 2.0f));

    currentNode = currentNode->parent;
  }

  SmoothPath();
}