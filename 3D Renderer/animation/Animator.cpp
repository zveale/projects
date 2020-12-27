#include "Animator.h"

Animator::Animator() {
  index = 0;
  repeatAnimation = false;
  animationComplete = false;
  animationTime = 0.0f;
  modelInverseBindTransform = glm::mat4(1.0f);
  currentAnimation = nullptr;
}

void Animator::Load(const aiScene* scene, const std::unordered_map<std::string, bool>& isJoint, bool repeatAnimation) {
  for (unsigned i = 0; i < scene->mNumAnimations; ++i) {
    Animation animation;
    animation.length = (float)scene->mAnimations[i]->mDuration;

    if (scene->mAnimations[i]->mNumChannels != 0) {
      unsigned numberOfPoses = scene->mAnimations[i]->mChannels[0]->mNumPositionKeys;

      for (unsigned j = 0; j < numberOfPoses; ++j) {
        std::unordered_map<std::string, JointTransform> pose;
        float timeStamp = (float)scene->mAnimations[i]->mChannels[0]->mPositionKeys[j].mTime;
        aiNodeAnim* channel = nullptr;

        for (unsigned k = 0; k < scene->mAnimations[i]->mNumChannels; ++k) {
          channel = scene->mAnimations[i]->mChannels[k];

          if (isJoint.find(channel->mNodeName.data) != isJoint.end() && isJoint.at(channel->mNodeName.data)) {
            glm::vec3 scaleKey = Cast_aiVec3DToGlmVec3(channel->mScalingKeys[j].mValue);
            glm::quat rotationKey = Cast_aiQuatToGlmQuat(channel->mRotationKeys[j].mValue);
            glm::vec3 positionKey = Cast_aiVec3DToGlmVec3(channel->mPositionKeys[j].mValue);

            pose[channel->mNodeName.data] = JointTransform(scaleKey, rotationKey, positionKey);
          }
        }

        animation.keyFrames[timeStamp] = KeyFrame(timeStamp, pose);
        animation.timestamps.emplace_back(timeStamp);
      }
    }

    animations.push_back(animation);
  }

  this->repeatAnimation = repeatAnimation;
  this->animationComplete = false;
  this->animationTime = 0.0f;
  this->currentAnimation = &animations.back();

  this->modelInverseBindTransform = Cast_aiMat4ToGlmMat4(scene->mRootNode->mTransformation);
}

void Animator::NextAnimation() {
  ++index;
  animationTime = 0.0f;
  animationComplete = false;
}

void Animator::SelectAnimation(const int poseIndex) {
  index = poseIndex;
  animationTime = 0.0f;
  animationComplete = false;
}

void Animator::CurrentAnimation() {
  animationTime = 0.0f;
  animationComplete = false;
}

bool Animator::AnimationComplete() const { return animationComplete; }

bool Animator::AnimationIsPercentComplete(float percent) {
  return (animationTime / currentAnimation->length) > percent;
}

void Animator::CalculateCurrentPose(Joint& joint, float dt) {
  index = index % animations.size();
  if (!animations.empty() && (index >= 0 && index < animations.size()))
    currentAnimation = &animations[index];

  IncreaseAnimationTime(dt);

  KeyFrame previous = (*currentAnimation->keyFrames.begin()).second;
  KeyFrame next = (*currentAnimation->keyFrames.begin()).second;
  FindPreviousAndNextFrames(previous, next);

  float progression = CalculateProgression(previous, next);

  std::unordered_map<std::string, glm::mat4> currentPose;
  InterpolatePoses(currentPose, previous, next, progression);

  ApplyPoseToJoints(currentPose, joint, glm::mat4(1.0f)); // TODO needs to be profiled
}

void Animator::IncreaseAnimationTime(float dt) {
  animationTime += dt;

  if (animationTime >= currentAnimation->length) {
    animationComplete = true;

    if (repeatAnimation)
      animationTime = 0.0f;
    else
      animationTime = currentAnimation->length;
  }
  else
    animationComplete = false;
}

void Animator::FindPreviousAndNextFrames(KeyFrame& previous, KeyFrame& next) {
  auto it = std::upper_bound(
    std::begin(currentAnimation->timestamps),
    std::end(currentAnimation->timestamps),
    animationTime, std::less_equal<float>());

  auto result = currentAnimation->keyFrames.find(*it);
  if (result != currentAnimation->keyFrames.end())
    next = currentAnimation->keyFrames[*it];

  result = currentAnimation->keyFrames.find(*--it);
  if (result != currentAnimation->keyFrames.end())
    previous = currentAnimation->keyFrames[*it];
  else
    previous = next;
}

float Animator::CalculateProgression(const KeyFrame& previousFrame, const KeyFrame& nextFrame) {
  if (nextFrame.timeStamp == previousFrame.timeStamp)
    return 1.0f;

  float totalTime = nextFrame.timeStamp - previousFrame.timeStamp;
  float currentTime = animationTime - previousFrame.timeStamp;
  return currentTime / totalTime;
}

void Animator::InterpolatePoses(std::unordered_map<std::string, glm::mat4>& currentPose,
  KeyFrame previousFrame, KeyFrame nextFrame, float progression) {
  JointTransform currentTransform;

  for (const std::pair<std::string, JointTransform>& pose : previousFrame.pose) {
    currentTransform.Interpolate(previousFrame.pose[pose.first], nextFrame.pose[pose.first], progression);
    currentPose.insert(std::pair<std::string, glm::mat4>(pose.first, currentTransform.GetLocalTransform()));
  }
}

void Animator::ApplyPoseToJoints(const std::unordered_map<std::string, glm::mat4>& currentPose, Joint& joint, glm::mat4 parentTransform) {
  glm::mat4 currentLocalTransform = glm::mat4(1.0f);

  auto pose = currentPose.find(joint.name);
  if (pose != currentPose.end())
    currentLocalTransform = pose->second;

  glm::mat4 currentTransform = parentTransform * currentLocalTransform;
  joint.animatedTransform = modelInverseBindTransform * currentTransform * joint.inverseBindTransform;

  for (unsigned i = 0; i < joint.childrenJoints.size(); ++i) {
    ApplyPoseToJoints(currentPose, joint.childrenJoints[i], currentTransform);
  }
}

glm::mat4 Animator::Cast_aiMat4ToGlmMat4(const aiMatrix4x4& ai_matr) {
  return glm::mat4(
    ai_matr.a1, ai_matr.b1, ai_matr.c1, ai_matr.d1,
    ai_matr.a2, ai_matr.b2, ai_matr.c2, ai_matr.d2,
    ai_matr.a3, ai_matr.b3, ai_matr.c3, ai_matr.d3,
    ai_matr.a4, ai_matr.b4, ai_matr.c4, ai_matr.d4);
}

glm::quat Animator::Cast_aiQuatToGlmQuat(const aiQuaternion& aiQuat) {
  return glm::quat(aiQuat.w, aiQuat.x, aiQuat.y, aiQuat.z);
}

glm::vec3 Animator::Cast_aiVec3DToGlmVec3(const aiVector3D& aiVec) {
  return glm::vec3(aiVec.x, aiVec.y, aiVec.z);
}