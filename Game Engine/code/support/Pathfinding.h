// Sebastian Lague A* Pathfinding Tutorial https://www.youtube.com/watch?v=-L-WgKMFuhE&list=PLFt_AvWsXl0cq5Umv3pMC9SPnKjfp9eGW

#ifndef PATHFINDING_H
#define PATHFINDING_H

#include <vector>
#include <list>
#include "glm\vec2.hpp"
#include "glm\vec3.hpp"

struct Node {
  Node();
  Node(int isWalkable, const glm::vec2& gridIndex);
  int FCost() const;

  Node* parent;
  int gCost;
  int hCost;

  bool isWalkable;
  glm::vec2 gridIndex;
};

class Pathfinding {
public:
  /*
  Load the binary file with walkable nodes. 1 represents walkable node.
  */
  void Load();

  void FindPath(const glm::vec3& startPosition, const glm::vec3& targetPosition);
  glm::vec3 FirstPosition();
  glm::vec3 SecondPostion();

  /*
  Return empty list.
  */
  bool NoPath();

  std::vector<glm::vec3> GetList() const;
  void AddPositionToList(const glm::vec3& position);

private:
  int gridWidth = 0, gridHeight = 0;
  std::vector<std::vector<Node>> nodeGrid;
  const int NON_DIAGONAL_COST = 10;
  const int DIAGONAL_COST = 14;
  std::list<glm::vec3> positionalList;

  /*
  Reduce the number of nodes that need to be traversed.
  */
  void SmoothPath();

  bool IsPositionWalkable(const glm::vec3& position, const glm::vec3& target);
  bool IsPositionInGridWalkable(const glm::vec3& position);
  Node* FindNodeInGrid(const glm::vec3& position);
  std::list<Node*> FindNeighboringNodes(Node* currentNode);
  int FindCostBetweenNodes(Node* nodeA, Node* nodeB);

  /*
  Traces the nodes from the target and creates a vector of positions to the targetPosition. If
  there is no path a vector of empty positions are returned.
  */
  void CalculatePathPostions(const glm::vec3& startPosition, const glm::vec3& targetPosition);
};

#endif // !PATHFINDING_H
