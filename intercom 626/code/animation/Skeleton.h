#ifndef SKELETON_H
#define SKELETON_H

#include <vector>
#include <unordered_map>
#include <string>
#include "assimp\scene.h"
#include "glm\mat4x4.hpp"
#include "Mesh.h"

struct Joint {
  unsigned index;
  std::string name;
  std::vector<Joint> childrenJoints;
  glm::mat4 inverseBindTransform;
  glm::mat4 animatedTransform;  // Stores the transform sent to shader.

  Joint();
  Joint(int index, const std::string& name, const glm::mat4& inverseBindTransform);
  void AddChild(const Joint& joint);
  Joint& GetLastChild();
};

class Skeleton {
public:
  Joint rootJoint;
  unsigned numJoints;
  std::vector<glm::mat4> jointMatrices;

  Skeleton();

  /*
  Loads the skeleton. Follows assimp documentation.
  Assimp documentation:
    a) Create a map or a similar container to store which nodes are necessary for the skeleton. Pre-initialise it for 
    all nodes with a "no".
    b) For each bone in the mesh:
      b1) Find the corresponding node in the scene's hierarchy by comparing their names.
      b2) Mark this node as "yes" in the necessityMap.
      b3) Mark all of its parents the same way until you 1) find the mesh's node or 2) the parent of the mesh's node.
    c) Recursively iterate over the node hierarchy
      c1) If the node is marked as necessary, copy it into the skeleton and check its children
      c2) If the node is marked as not necessary, skip it and do not iterate over its children.
  */
  void Load(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene, const aiNode* node);

  /*
  Animator uses joint map to identify associated key frames.
  */
  const std::unordered_map<std::string, bool>& JointsUsed() const;

  /*
  Updates an array of animated transforms to be sent to the shader. Called by animated model.
  */
  void AddJointsToArray(const Joint& joint);

private:
  std::unordered_map<std::string, unsigned> jointIndex;
  std::unordered_map<std::string, bool> isJoint;  // Used to check key frames in animator.
  std::unordered_map<std::string, glm::mat4> inverseBindTransforms;

  void AddJointWeightToVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const unsigned jointId);

  /*
  Assimp documentation:
    a) Create a map or a similar container to store which nodes are necessary for the skeleton. Pre-initialise it for 
    all nodes with a "no".
  */
  void IntializeSkeletonHierarchy(const aiNode* node);

  /*
  Assimp documentation:
    b) For each bone in the mesh:
      b1) Find the corresponding node in the scene's hierarchy by comparing their names. **step is done in Load()**
      b2) Mark this node as "yes" in the necessityMap.
      b3) Mark all of its parents the same way until you 1) find the mesh's node or 2) the parent of the mesh's node.
  */
  aiNode* FindSkeletonHierarchy(aiNode* node, const aiNode* scene);

  /*
  Assimp documentation:
    c) Recursively iterate over the node hierarchy
      c1) If the node is marked as necessary, copy it into the skeleton and check its children
      c2) If the node is marked as not necessary, skip it and do not iterate over its children.
  */
  void CreateSkeleton(aiNode* node, Joint& joint, unsigned& jointCount);

  glm::mat4 Cast_aiMat4ToGlmMat4(const aiMatrix4x4& ai_matr);
};

#endif // !SKELETON_H
