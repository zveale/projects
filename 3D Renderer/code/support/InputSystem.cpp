#include "InputSystem.h"
#include "Global.h"
#include <cstring>

InputSystem::InputSystem() {
  quitFlag = false;
  numKeys = 0;
  mouseX = 0;
  mouseY = 0;
  mouseScrollValue = 0;
}

void InputSystem::StartUp() {
  if (SDL_NumJoysticks() > 0) {
    gameController = SDL_GameControllerOpen(0);

    haptic = SDL_HapticOpen(0);
    SDL_HapticRumbleInit(haptic);
  }

  quitFlag = false;
  keyboardState = SDL_GetKeyboardState(&numKeys);
  previousKeyboardState = new Uint8[numKeys];

  SDL_EventState(SDL_KEYDOWN, SDL_IGNORE);
  SDL_EventState(SDL_KEYUP, SDL_IGNORE);
  SDL_EventState(SDL_TEXTEDITING, SDL_IGNORE);
  SDL_EventState(SDL_TEXTINPUT, SDL_IGNORE);
  SDL_EventState(SDL_KEYMAPCHANGED, SDL_IGNORE);
  SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
  SDL_JoystickEventState(SDL_IGNORE);
  SDL_SetRelativeMouseMode(SDL_TRUE);
}

void InputSystem::ShutDown() {
  delete[] previousKeyboardState;
  SDL_HapticClose(haptic);
  SDL_GameControllerClose(gameController);
}

void InputSystem::Process() {
  memcpy(previousKeyboardState, keyboardState, numKeys);

  while (SDL_PollEvent(&sdlEvent) != 0) {
    if (sdlEvent.type == SDL_WINDOWEVENT)
      if (sdlEvent.window.event == SDL_WINDOWEVENT_RESIZED) {
        GLOBAL::WINDOW::WIDTH = sdlEvent.window.data1;
        GLOBAL::WINDOW::HEIGHT = sdlEvent.window.data2;
        GLOBAL::WINDOW::RESIZE = true;
      }

    if (sdlEvent.type == SDL_MOUSEWHEEL)
      mouseScrollValue = sdlEvent.wheel.y;

    if (sdlEvent.type == SDL_QUIT)
      quitFlag = true;
  }

  SDL_GetRelativeMouseState(&mouseX, &mouseY);

  if (keyboardState[SDL_SCANCODE_ESCAPE])
    quitFlag = true;

  if (SDL_NumJoysticks() > 0) {
    gameController = SDL_GameControllerOpen(0);
    //SDL_JoystickUpdate();
    GamepadUpdate();

    if (controllerState.A_BUTTON) {
      SDL_HapticRumblePlay(haptic, 1, 2000);
    }
  }
}

bool InputSystem::KeyPressed(const SDL_Scancode& scanCode) { return keyboardState[scanCode] && !previousKeyboardState[scanCode]; }

bool InputSystem::KeyRelease(const SDL_Scancode& scanCode) { return !keyboardState[scanCode] && previousKeyboardState[scanCode]; }

bool InputSystem::KeyHeld(const SDL_Scancode& scanCode) { return keyboardState[scanCode] && previousKeyboardState[scanCode]; }

const Uint8* InputSystem::GetReferenceToKeyboardState() const { return previousKeyboardState; }

int* InputSystem::GetReferenceToMouseX() { return &mouseX; }

int* InputSystem::GetReferenceToMouseY() { return &mouseY; }

int* InputSystem::GetReferenceToMouseScrollvalue() { return &mouseScrollValue; }

const ControllerState InputSystem::GetGamepadState() const { return controllerState; }

bool InputSystem::Quit() { return quitFlag; }

void InputSystem::GamepadUpdate() {
  controllerState.LEFT_STICK_X_AXIS = SDL_GameControllerGetAxis(gameController, SDL_CONTROLLER_AXIS_LEFTX);
  controllerState.LEFT_STICK_Y_AXIS = SDL_GameControllerGetAxis(gameController, SDL_CONTROLLER_AXIS_LEFTY);
  controllerState.RIGHT_STICK_X_AXIS = SDL_GameControllerGetAxis(gameController, SDL_CONTROLLER_AXIS_RIGHTX);
  controllerState.RIGHT_STICK_Y_AXIS = SDL_GameControllerGetAxis(gameController, SDL_CONTROLLER_AXIS_RIGHTY);
  controllerState.LEFT_TRIGGER = SDL_GameControllerGetAxis(gameController, SDL_CONTROLLER_AXIS_TRIGGERLEFT);
  controllerState.RIGHT_TRIGGER = SDL_GameControllerGetAxis(gameController, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
  controllerState.A_BUTTON = SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_A);
  controllerState.B_BUTTON = SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_B);
  controllerState.X_BUTTON = SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_X);
  controllerState.Y_BUTTON = SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_Y);
  controllerState.BACK_BUTTON = SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_BACK);
  controllerState.GUIDE_BUTTON = SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_GUIDE);
  controllerState.START_BUTTON = SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_START);
  controllerState.LEFT_STICK_BUTTON = SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_LEFTSTICK);
  controllerState.RIGHT_STICK_BUTTON = SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_RIGHTSTICK);
  controllerState.LEFT_SHOULDER_BUTTON = SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
  controllerState.RIGHT_SHOULDER_BUTTON = SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
  controllerState.DPAD_UP = SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_DPAD_UP);
  controllerState.DPAD_DOWN = SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
  controllerState.DPAD_LEFT = SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
  controllerState.DPAD_RIGHT = SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
}
