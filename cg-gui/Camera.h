#pragma once

#include "InputController.h"

class Camera {
private:
	InputController* input = InputController::getInstance();

	// Window parameters
	int width = 1280, height = 720;

	// Camera speed
	float cameraSpeed = 0.05f;
	float cameraSpeedUp = 2.f;
    float cameraVerticalSpeed = 0.0025f;
	float cameraHorizontalSpeed = 0.0025f;
	float cameraRotationSpeed = 0.075f;

	// Camera rotation
	float pitch = 0.f;
	float yaw = -90.f;
	
	// Camera scale
	float fovy = 45.f;
	float scaleSpeed = 0.1f;

	// Camera view
	float near = 0.1f;
	float far = 1000.f;

	// Camera parameters
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	glm::vec3 cameraRight;

	glm::mat4 projection;
	glm::mat4 view;

	/* Functions */
	void updateCameraMovement(CameraMovement cameraMovement);
	void updateCameraRatation(glm::vec2 mouseMoveDelta);
	void updateCameraScale(float mouseScrollDelta);
	void updateCameraScrollMovement(glm::vec2 mouseMoveDelta);

public:
	/* Constructor and Deconstructor */
	Camera();
    ~Camera();

	/* Functions */
	void update();
	void updateWindowSize(int width, int height);

	/* Getter and Setter */
	const glm::vec3& getCameraPos() const;
	const glm::mat4& getProjectionMatrix() const;
	const glm::mat4& getViewMatrix() const;
};

