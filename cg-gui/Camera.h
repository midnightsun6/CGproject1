#pragma once

#include "ControllerManager.h"

class Camera {
private:
	InputController* input = ControllerManager::getInputController();

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

	// Camera parameters
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	glm::vec3 cameraRight;

	glm::mat4 projection;
	glm::mat4 view;

	/* Functions */
	void UpdateCameraMovement(CameraMovement cameraMovement);
	void UpdateCameraRatation(glm::vec2 mouseMoveDelta);
	void UpdateCameraScale(float mouseScrollDelta);
	void UpdateCameraScrollMovement(glm::vec2 mouseMoveDelta);

public:
	/* Constructor and Deconstructor */
	Camera();
    ~Camera();

	/* Functions */
	void Update();
	void UpdateWindowSize(int width, int height);

	/* Getter and Setter */
	glm::vec3 getCameraPos();
	glm::mat4 getProjectionMatrix();
	glm::mat4 getViewMatrix();
};

