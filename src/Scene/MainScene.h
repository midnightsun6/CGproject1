#pragma once

#include "../../cg-gui/Model.h"
#include "../../cg-gui/Camera.h"
#include "../../cg-gui/GLBaseObject.h"

class MainScene {
private:
	float totalTime;

	Camera camera;

	/* Base Objects (temp) */
	GLBaseObject grid, box;
	std::vector<GLBaseObject> spheres;

	/* Object */
	Model model;
	Shader modelShader;

public:
	MainScene();
	~MainScene();

	/*
		GLFWkey function cannot use the reference lambda function => cannot access InputController in App.h.
		Temporary sol: Set InputController as public.
	*/
	InputController* input = ControllerManager::getInputController();

	bool Initialize();
	void Update(double dt);
	void Render();

	void OnResize(int width, int height);

	void setBox(float size);
	void setGrid();
	void addSphere(float radius, int slices, int stacks, glm::vec3 color);
};

