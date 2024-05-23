#include "Camera.h"

Camera::Camera() {
	cameraPos = glm::vec3(0.0f, 15.0f, 20.0f);
	cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	projection = glm::perspective(fovy, (float)width / height, near, far);
}

Camera::~Camera() {}

void Camera::updateWindowSize(int width, int height) {
	this->width = width;
	this->height = height;
}

void Camera::updateCameraRatation(glm::vec2 mouseMoveDelta) {
    // Camera rotation
    pitch += mouseMoveDelta.y * cameraRotationSpeed;
    pitch = glm::clamp(pitch, -89.f, 89.f);
    yaw += mouseMoveDelta.x * cameraRotationSpeed;
    yaw = glm::mod(yaw, 360.f);

	// Compute new front.
    glm::vec3 newCameraFront;
    newCameraFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newCameraFront.y = -sin(glm::radians(pitch));
    newCameraFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = newCameraFront;
	cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));
}

void Camera::updateCameraScale(float mouseScrollDelta) {
    // Camera scale
    fovy += -mouseScrollDelta * scaleSpeed; // Upside down
    fovy = glm::clamp(fovy, 44.f, 47.f);
}

void Camera::updateCameraScrollMovement(glm::vec2 mouseMoveDelta) {
	cameraPos += cameraSpeed * cameraHorizontalSpeed * fovy * -mouseMoveDelta.x * cameraRight;
    cameraPos += cameraSpeed * cameraVerticalSpeed * fovy * mouseMoveDelta.y * cameraUp;
}

void Camera::updateCameraMovement(CameraMovement cameraMovement) {
	// Camera speed up
	float moveSpeed = cameraSpeed * 5;
	if (input->getCameraMovementInput(CameraMovement::SPEED_UP)) {
		moveSpeed *= cameraSpeedUp;
	}

	// Camera movement
	switch (cameraMovement) {
		case FORWARD:
			cameraPos += moveSpeed * cameraFront;
			break;
		case BACKWARD:
			cameraPos -= moveSpeed * cameraFront;
			break;
		case LEFTSIDE:
			cameraPos -= moveSpeed * cameraRight;
			break;
		case RIGHTSIDE:
			cameraPos += moveSpeed * cameraRight;
			break;
		default:
			break;
	}
}

void Camera::update() {
	// WASD
	if (input->getCameraMovementInput(CameraMovement::FORWARD)) {
		updateCameraMovement(CameraMovement::FORWARD);
	}
	if (input->getCameraMovementInput(CameraMovement::BACKWARD)) {
		updateCameraMovement(CameraMovement::BACKWARD);
	}
	if (input->getCameraMovementInput(CameraMovement::LEFTSIDE)) {
		updateCameraMovement(CameraMovement::LEFTSIDE);
	}
	if (input->getCameraMovementInput(CameraMovement::RIGHTSIDE)) {
		updateCameraMovement(CameraMovement::RIGHTSIDE);
	}

	// Scroll (vertical)
	if (input->getCameraMovementInput(CameraMovement::VERTICAL)) {
		updateCameraScrollMovement(input->getCursorMoveDelta());
	}

	// Rotation
	if (input->getCameraMovementInput(CameraMovement::ROTATION)) {
		updateCameraRatation(input->getCursorMoveDelta());
	}

	// Scale
	updateCameraScale(input->getScrollDeltaY());

    // Update camera matrix
	float apect = (height == 0 ? 1 : (float)width / height);
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	projection = glm::perspective(fovy, apect, near, far);
}

const glm::vec3& Camera::getCameraPos() const {
	return cameraPos;
}

const glm::vec3& Camera::getCameraFront() const {
	return cameraFront;
}

const glm::mat4& Camera::getProjectionMatrix() const {
	return projection;
}

const glm::mat4& Camera::getViewMatrix() const {
	return view;
}


