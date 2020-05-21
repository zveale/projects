#ifndef SWITCHES_H
#define SWITCHES_H

#include "GameObject.h"
#include "AnimatedModel.h"

class Switches: public GameObject {
public:
  Switches();
  void Load();
  void Update(float dt = 0.0f);
  void Draw(ShaderProgram& shaderProgram, int index = 0);
  void SendShaderData(ShaderProgram& shaderProgram, const int index = 0);
  void Delete();
  const unsigned NumElements();

  bool AnimationCompleteDoorIsOpen(int doorIndex);
  bool AnimationIsPercentCompleteDoorIsOpen(int doorIndex, const float percent);
  void NextAnimation(int doorIndex);

  void ToggleLeftSwitch();
  void ToggleCenterSwitch();
  void ToggleRightSwitch();

  bool IsLeftSwitchOn();
  bool IsCenterSwitchOn();
  bool IsRightSwitchOn();

  void SetSwitchToggleFlag(bool switchesToggled);
  bool SwitchesToggled();

private:
  const static unsigned numSwitches = 3; // [0] left, [1] center, [2] right
  AnimatedModel models[numSwitches];
  bool switchIsOn[numSwitches];
  bool switchesToggled;
};

#endif // !SWITCHES_H
