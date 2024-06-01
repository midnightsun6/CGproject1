#include "InputController.h"

InputController InputController::input;

InputController::InputController() {
	isMouseHeld.resize(MouseHeld::NO_HELD);
	isCameraMoveHeld.resize(CameraMovement::NO_MOVE);
	isKeyPress = std::vector<bool>(26, false);
}

InputController* InputController::getInstance() {
	return &input;
}

void InputController::OnKeyboard(int key, int action) {
	bool isHeld = false;
	switch (action) {
		case GLFW_PRESS: case GLFW_REPEAT:
			isHeld = true;
			break;
		default:
			isHeld = false;
			break;
	}

	if ('A' <= key && key <= 'Z') {
		isKeyPress[key - 'A'] = isHeld;
	}

	switch (key) {
		case GLFW_KEY_W:
			isCameraMoveHeld[CameraMovement::FORWARD] = isHeld;
			break;
		case GLFW_KEY_S:
			isCameraMoveHeld[CameraMovement::BACKWARD] = isHeld;
			break;
		case GLFW_KEY_A:
			isCameraMoveHeld[CameraMovement::LEFTSIDE] = isHeld;
			break;
		case GLFW_KEY_D:
			isCameraMoveHeld[CameraMovement::RIGHTSIDE] = isHeld;
			break;
		case GLFW_KEY_LEFT_SHIFT:
			isCameraMoveHeld[CameraMovement::SPEED_UP] = isHeld;
		default:
			break;
	}
}

void InputController::OnCursorMove(double xpos, double ypos) {
	glm::vec2 nowPos = glm::vec2(xpos, ypos);
	cursorMoveDelta = nowPos - cursorLastPos;
	cursorLastPos = nowPos;
}

void InputController::OnMouseButton(int button, int action, int mods) {
	bool isHeld = false;
	switch (action) {
		case GLFW_PRESS: case GLFW_REPEAT:
			isHeld = true;
			break;
		default:
			isHeld = false;
			break;
	}

	switch (button) {
		case GLFW_MOUSE_BUTTON_RIGHT:
			isCameraMoveHeld[CameraMovement::ROTATION] = isHeld;
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			isCameraMoveHeld[CameraMovement::VERTICAL] = isHeld;
			break;
		default:
			break;
	}
}

void InputController::OnMouseScroll(double xoffset, double yoffset) {
	scrollDeltaY = yoffset;
}

glm::vec2 InputController::getCursorMoveDelta() {
	glm::vec2 delta = cursorMoveDelta;
	cursorMoveDelta = glm::vec2(0);
	return delta;
}

float InputController::getScrollDeltaY() {
	int dy = scrollDeltaY;
	scrollDeltaY = 0;
	return dy;
}

bool InputController::getCameraMovementInput(CameraMovement cm) {
	return isCameraMoveHeld[cm];
}

bool InputController::getMouseHeld(MouseHeld mh) {
	return isMouseHeld[mh];
}

bool InputController::getKeyPress(char key) {
	if (std::isupper(key)) {
		return isKeyPress[key - 'A'];
	}
	else if (std::islower(key)) {
		return isKeyPress[key - 'a'];
	}
	return false;
}
