#pragma once
#include "Definition.h"

enum MouseHeld {
	LEFT,
	RIGHT,
	MIDDLE,
	NO_HELD,
};

enum CameraMovement {
	FORWARD,
	BACKWARD,
	LEFTSIDE,
	RIGHTSIDE,
	VERTICAL,
	ROTATION,
	SPEED_UP,
	NO_MOVE
};

class InputController {
private:
	static InputController input;	// A singleton InputController.

	glm::vec2 cursorLastPos, cursorMoveDelta;
	float scrollDeltaY;
	std::vector<bool> isMouseHeld;
	std::vector<bool> isCameraMoveHeld;
	std::vector<bool> isKeyPress;

	InputController();

public:
	static InputController* getInstance();

	void OnKeyboard(int key, int action);
	void OnCursorMove(double xpos, double ypos);
	void OnMouseButton(int button, int action, int mods);
	void OnMouseScroll(double xoffset, double yoffset);

	glm::vec2 getCursorMoveDelta();
	float getScrollDeltaY();
	bool getCameraMovementInput(CameraMovement cm);
	bool getMouseHeld(MouseHeld mh);
	bool getKeyPress(char key);
};