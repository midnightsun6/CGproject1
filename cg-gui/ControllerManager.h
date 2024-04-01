#pragma once
#include "InputController.h"

// Singleton system for controllers.
class ControllerManager {
private:
	static InputController* input;

public:
	static InputController* getInputController();
};

