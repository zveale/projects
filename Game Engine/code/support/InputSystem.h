#ifndef INPUTSYSTEM_H
#define INPUTSYSTEM_H

#include "sdl\SDL.h"

struct ControllerState {
  short LEFT_STICK_X_AXIS = 0;
  short LEFT_STICK_Y_AXIS = 0;
  short RIGHT_STICK_X_AXIS = 0;
  short RIGHT_STICK_Y_AXIS = 0;

  short LEFT_TRIGGER = 0;
  short RIGHT_TRIGGER = 0;

  Uint8  A_BUTTON = 0;
  Uint8  B_BUTTON = 0;
  Uint8  X_BUTTON = 0;
  Uint8  Y_BUTTON = 0;

  Uint8  BACK_BUTTON = 0;
  Uint8  GUIDE_BUTTON = 0;
  Uint8  START_BUTTON = 0;

  Uint8  LEFT_STICK_BUTTON = 0;
  Uint8  RIGHT_STICK_BUTTON = 0;

  Uint8  LEFT_SHOULDER_BUTTON = 0;
  Uint8  RIGHT_SHOULDER_BUTTON = 0;

  Uint8  DPAD_UP = 0;
  Uint8  DPAD_DOWN = 0;
  Uint8  DPAD_LEFT = 0;
  Uint8  DPAD_RIGHT = 0;
};

class InputSystem {
public:
  InputSystem();

  void StartUp();
  void ShutDown();
  void Process();

  bool KeyPressed(const SDL_Scancode& scanCode);
  bool KeyRelease(const SDL_Scancode& scanCode);
  bool KeyHeld(const SDL_Scancode& scanCode);

  const Uint8* GetReferenceToKeyboardState() const;
  int* GetReferenceToMouseX();
  int* GetReferenceToMouseY();
  int* GetReferenceToMouseScrollvalue();
  const ControllerState GetGamepadState() const;
  bool Quit();

private:
  bool quitFlag;
  SDL_Event sdlEvent;

  const Uint8* keyboardState;
  Uint8* previousKeyboardState;
  int numKeys;

  ControllerState controllerState;
  SDL_GameController* gameController;
  SDL_Haptic *haptic;
  int mouseX, mouseY;
  int mouseScrollValue;

  void GamepadUpdate();
};
#endif // !INPUTSYSTEM_H
