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
	std::unordered_map<std::string, Model> models;
	Shader modelShader, baseObjShader;

public:
	MainScene();
	~MainScene();

	/*
		GLFWkey function cannot use the reference lambda function => cannot access InputController in App.h.
		Temporary sol: Set InputController as public.
	*/
	InputController* input = ControllerManager::getInputController();

	/* Scene functions */
	bool Initialize();
	void Update(double dt);
	void Render();
	void OnResize(int width, int height);

	/* OpenGL Base Object functions */
	void setBox(float size);
	void setGrid();
	void addSphere(float radius, int slices, int stacks, glm::vec3 color);

	/* OpenGL Model functions */
	void loadModel(const char* path, const char* name);
	void importAnimation(const char* name, const char* path);
	void exportAnimation(const char* objname, const char* filename, const char* animation);
	void addAnimation(const char* name, AnimationClip clip);
	void deleteAnimation(const char* objname, const char* animation);
	void playAnimation(const char* objName, const char* animation);

	const std::unordered_map<std::string, Model>& getModels() const;
	const Model& getModel(const char* name) const;
};

