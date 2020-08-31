/*
Resources:
  ThinMatrix OpenGL Skeletal Animation Tutorials https://www.youtube.com/watch?v=f3Cr8Yx3GGA&list=PLRIWtICgwaX2tKWCxdeB7Wv_rTET9JtWW
  OGLdev Skeletal Animation With Assimp http://ogldev.atspace.co.uk/www/tutorial38/tutorial38.html
*/

#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <unordered_map>
#include <string>
#include <vector>
#include <iterator>
#include <functional>
#include <algorithm>
#include "Animation.h"
#include "Skeleton.h"
#include "glm\mat4x4.hpp"
#include "assimp/scene.h"

class Animator {
public:
  Animator();

  /*
  Load all animation data, key frames and time stamps. Push onto animations.
  */
  void Load(const aiScene* scene, const std::unordered_map<std::string, bool>& isJoint, bool repeatAnimation);

  void NextAnimation();
  void SelectAnimation(const int poseIndex);
  void CurrentAnimation();
  bool AnimationComplete() const;
  bool AnimationIsPercentComplete(float percent);

  /*
  Called every frame. Progress animation, find previous and next frames, interpolate, and 
  apply to animated transform in joint transform.
  */
  void CalculateCurrentPose(Joint& joint, float dt);

private:
  unsigned short int index;
  bool repeatAnimation;
  bool animationComplete;
  float animationTime;
  glm::mat4 modelInverseBindTransform;

  Animation* currentAnimation;
  std::vector<Animation> animations;

  void IncreaseAnimationTime(float dt);

  /*
  Search through all of the current animations time stamps. Assign previous and next to the time stamps less then and 
  greater than current animation time.
  */
  void FindPreviousAndNextFrames(KeyFrame& previous, KeyFrame& next);

  /*
  Check how far into the current keyframe the animation is.
  */
  float CalculateProgression(const KeyFrame& previousFrame, const KeyFrame& nextFrame);

  /*
  Update the current pose with interpolated transforms created from the previous keyframe and next keyframe.
  */
  void InterpolatePoses(std::unordered_map<std::string, glm::mat4>& currentPose, KeyFrame previousFrame, KeyFrame nextFrame, float progression);
  
  /*
  Convert joint transforms of current pose from local to world space.
  */
  void ApplyPoseToJoints(const std::unordered_map<std::string, glm::mat4>& currentPose, Joint& joint, glm::mat4 parentTransform);
  
  glm::mat4 Cast_aiMat4ToGlmMat4(const aiMatrix4x4& ai_matr);
  glm::quat Cast_aiQuatToGlmQuat(const aiQuaternion& aiQuat);
  glm::vec3 Cast_aiVec3DToGlmVec3(const aiVector3D& aiVec);
};

#endif // !ANIMATOR_H
