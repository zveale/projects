/*
Resources:
  ThinMatrix OpenGL Skeletal Animation Tutorials https://www.youtube.com/watch?v=f3Cr8Yx3GGA&list=PLRIWtICgwaX2tKWCxdeB7Wv_rTET9JtWW
  OGLdev Skeletal Animation With Assimp http://ogldev.atspace.co.uk/www/tutorial38/tutorial38.html
*/

#ifndef ANIMATION_H
#define ANIMATION_H

#include <map>
#include <unordered_map>
#include <string>
#include <vector>
#include "glm\vec3.hpp"
#include "glm\gtc\quaternion.hpp"
#include "glm\mat4x4.hpp"

struct JointTransform {
  glm::vec3 scale;
  glm::quat rotation;
  glm::vec3 position;

  JointTransform();
  JointTransform(glm::vec3 scale, glm::quat rotation, glm::vec3 position);
  glm::mat4 GetLocalTransform();
  void Interpolate(const JointTransform& frameA, const JointTransform& frameB, float progression);
  glm::vec3 Interpolate(glm::vec3 start, glm::vec3 end, float progression);
};


struct KeyFrame {
  float timeStamp;
  std::unordered_map<std::string, JointTransform> pose;

  KeyFrame();
  KeyFrame(float timeStamp, const std::unordered_map<std::string, JointTransform>& jointKeyFrames);
};


struct Animation {
  float length;
  std::unordered_map<float, KeyFrame> keyFrames;
  std::vector<float> timestamps;

  Animation();
  Animation(float lengthInSeconds, const std::unordered_map<float, KeyFrame>& keyFrames);
};

#endif // !ANIMATION_H
