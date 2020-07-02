#include "Animation.h"

// JointTransform
JointTransform::JointTransform() {
  this->scale = glm::vec3(1.0f);
  this->rotation = glm::quat(1.0f, 1.0f, 1.0f, 1.0f);
  this->position = glm::vec3(1.0f);
}

JointTransform::JointTransform(glm::vec3 scale, glm::quat rotation, glm::vec3 position) {
  this->scale = glm::vec3(1.0f);
  this->rotation = rotation;
  this->position = position;
}

glm::mat4 JointTransform::GetLocalTransform() {
  glm::mat4 matrix = glm::mat4(1.0f);
  matrix = glm::translate(matrix, position);
  matrix = matrix * glm::mat4_cast(rotation);
  matrix = glm::scale(matrix, glm::vec3(1.0f));
  return matrix;
}

void JointTransform::Interpolate(const JointTransform& frameA, const JointTransform& frameB, float progression) {
  this->scale = glm::mix(frameA.scale, frameB.scale, progression);
  this->rotation = glm::slerp(frameA.rotation, frameB.rotation, progression);
  this->position = glm::mix(frameA.position, frameB.position, progression);
}

glm::vec3 JointTransform::Interpolate(glm::vec3 start, glm::vec3 end, float progression) {
  return glm::mix(start, end, progression);
}

// KeyFrame
KeyFrame::KeyFrame() { timeStamp = 0.0f; }

KeyFrame::KeyFrame(float timeStamp, const std::unordered_map<std::string, JointTransform>& jointKeyFrames) {
  this->timeStamp = timeStamp;
  this->pose = jointKeyFrames;
}

// Animation
Animation::Animation() { length = 0.0f; }

Animation::Animation(float lengthInSeconds, const std::unordered_map<float, KeyFrame>& keyFrames) :
  length(lengthInSeconds), keyFrames(keyFrames) {}

