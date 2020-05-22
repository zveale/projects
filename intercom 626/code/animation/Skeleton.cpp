#include "Skeleton.h"

// Joint
Joint::Joint() {
  this->index = 9999;
  this->name = "NONE";
  this->inverseBindTransform = glm::mat4(1.0f);
  this->animatedTransform = glm::mat4(1.0f);
}
Joint::Joint(int index, const std::string& name, const glm::mat4& inverseBindTransform) :
  index(index), name(name), inverseBindTransform(inverseBindTransform) {
  this->animatedTransform = glm::mat4(1.0f);
}

void Joint::AddChild(const Joint& joint) { childrenJoints.push_back(joint); }

Joint& Joint::GetLastChild() {  return childrenJoints.back(); }

// Skeleton
Skeleton::Skeleton() : numJoints(0) {}

void Skeleton::Load(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene, const aiNode* node) {
  IntializeSkeletonHierarchy(scene->mRootNode);

  aiNode* root = nullptr;
  for (unsigned i = 0; i < mesh->mNumBones; i++) {
    const char* jointName = mesh->mBones[i]->mName.C_Str();

    inverseBindTransforms[jointName] = Cast_aiMat4ToGlmMat4(mesh->mBones[i]->mOffsetMatrix);
    isJoint[jointName] = true;
    jointIndex[jointName] = i;


    // Assimp documentation : b1) Find the corresponding node in the scene's hierarchy by comparing their names.
    root = FindSkeletonHierarchy(scene->mRootNode->FindNode(jointName), scene->mRootNode);

    AddJointWeightToVertices(vertices, mesh, i);
  }

  if (root != nullptr)
    rootJoint.name = root->mName.C_Str();
  else
    printf("Skeleton root is null in %s\n", scene->mRootNode->mName.C_Str());

  CreateSkeleton(root, rootJoint, numJoints);

  jointMatrices.resize(numJoints);
  AddJointsToArray(rootJoint);
}

const std::unordered_map<std::string, bool>& Skeleton::JointsUsed() const { return isJoint; }

void Skeleton::AddJointsToArray(const Joint& joint) {
  if (jointIndex.find(joint.name) != jointIndex.end())
    jointMatrices[jointIndex[joint.name]] = joint.animatedTransform;

  for (const Joint& childJoint : joint.childrenJoints)
    AddJointsToArray(childJoint);
}

void Skeleton::AddJointWeightToVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const unsigned jointId) {
  for (unsigned j = 0; j < mesh->mBones[jointId]->mNumWeights; j++) {
    unsigned vertex_id = mesh->mBones[jointId]->mWeights[j].mVertexId;
    float weight = mesh->mBones[jointId]->mWeights[j].mWeight;
    vertices[vertex_id].AddJointData(jointId, weight);
  }
}

void Skeleton::IntializeSkeletonHierarchy(const aiNode* node) {
  isJoint[node->mName.C_Str()] = false;

  for (unsigned i = 0; i < node->mNumChildren; ++i)
    IntializeSkeletonHierarchy(node->mChildren[i]);
}

aiNode* Skeleton::FindSkeletonHierarchy(aiNode* node, const aiNode* scene) {
  aiNode* parent = node->mParent;
  if (parent == scene || parent == nullptr)
    return node;

  isJoint[parent->mName.C_Str()] = true;
  return FindSkeletonHierarchy(parent, scene);
}

void Skeleton::CreateSkeleton(aiNode* node, Joint& joint, unsigned& jointCount) {
  for (unsigned i = 0; i < node->mNumChildren; ++i) {
    const char* childJointName = node->mChildren[i]->mName.C_Str();

    if (isJoint[childJointName]) {
      Joint childJoint;
      childJoint.index = jointCount++;
      childJoint.name = childJointName;
      if (inverseBindTransforms.find(childJointName) != inverseBindTransforms.end())
        childJoint.inverseBindTransform = inverseBindTransforms[childJointName];

      joint.AddChild(childJoint);
      CreateSkeleton(node->mChildren[i], joint.GetLastChild(), jointCount);
    }
    else
      CreateSkeleton(node->mChildren[i], joint, jointCount);
  }
}

glm::mat4 Skeleton::Cast_aiMat4ToGlmMat4(const aiMatrix4x4& ai_matr) {
  return glm::mat4(
    ai_matr.a1, ai_matr.b1, ai_matr.c1, ai_matr.d1,
    ai_matr.a2, ai_matr.b2, ai_matr.c2, ai_matr.d2,
    ai_matr.a3, ai_matr.b3, ai_matr.c3, ai_matr.d3,
    ai_matr.a4, ai_matr.b4, ai_matr.c4, ai_matr.d4);
}