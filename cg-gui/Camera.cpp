#include "Camera.h"

Camera::Camera() {
	cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	projection = glm::perspective(fovy, (float)width / height, 0.1f, 100.f);
}

Camera::~Camera() {}

void Camera::UpdateWindowSize(int width, int height) {
	this->width = width;
	this->height = height;
}

void Camera::UpdateCameraRatation(glm::vec2 mouseMoveDelta) {
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

void Camera::UpdateCameraScale(float mouseScrollDelta) {
    // Camera scale
    fovy += -mouseScrollDelta * scaleSpeed; // Upside down
    fovy = glm::clamp(fovy, 44.f, 47.f);
}

void Camera::UpdateCameraScrollMovement(glm::vec2 mouseMoveDelta) {
	cameraPos += cameraSpeed * cameraHorizontalSpeed * fovy * -mouseMoveDelta.x * cameraRight;
    cameraPos += cameraSpeed * cameraVerticalSpeed * fovy * mouseMoveDelta.y * cameraUp;
}

void Camera::UpdateCameraMovement(CameraMovement cameraMovement) {
	// Camera speed up
	float moveSpeed = cameraSpeed;
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

void Camera::Update() {
	// WASD
	if (input->getCameraMovementInput(CameraMovement::FORWARD)) {
		UpdateCameraMovement(CameraMovement::FORWARD);
	}
	if (input->getCameraMovementInput(CameraMovement::BACKWARD)) {
		UpdateCameraMovement(CameraMovement::BACKWARD);
	}
	if (input->getCameraMovementInput(CameraMovement::LEFTSIDE)) {
		UpdateCameraMovement(CameraMovement::LEFTSIDE);
	}
	if (input->getCameraMovementInput(CameraMovement::RIGHTSIDE)) {
		UpdateCameraMovement(CameraMovement::RIGHTSIDE);
	}

	// Scroll (vertical)
	if (input->getCameraMovementInput(CameraMovement::VERTICAL)) {
		UpdateCameraScrollMovement(input->getCursorMoveDelta());
	}

	// Rotation
	if (input->getCameraMovementInput(CameraMovement::ROTATION)) {
		UpdateCameraRatation(input->getCursorMoveDelta());
	}

	// Scale
	UpdateCameraScale(input->getScrollDeltaY());

    // Update camera matrix
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	projection = glm::perspective(fovy, (float)width / height, 0.1f, 100.f);
}

glm::mat4 Camera::getProjectionMatrix() {
	return projection;
}

glm::mat4 Camera::getViewMatrix() {
	return view;
}


