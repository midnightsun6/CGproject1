#include "ControllerManager.h"

InputController* ControllerManager::input = InputController::getInstance();

InputController* ControllerManager::getInputController() {
	return input;
}
