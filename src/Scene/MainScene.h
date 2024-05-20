#pragma once

#include "../../cg-gui/Model.h"
#include "../../cg-gui/Camera.h"
#include "../../cg-gui/GLBaseObject.h"
#include "../../cg-gui/Skybox.h"
#include "../../cg-gui/Terrian.h"
#include "../../cg-gui/Grass.h"
#include "../../cg-gui/ParticleSystem.h"

class MainScene {
private:
	float totalTime;

	int screenWidth = 1280;
	int screenHeight = 760;

	Camera camera;

	/* Background */
	Skybox skybox;
	Terrian terrian;
	Grass grass;

	/* Base Objects (temp) */
	GLBaseObject grid, box;
	std::vector<GLBaseObject> spheres;

	/* Object */
	std::unordered_map<std::string, Model> models;

	/* Particle */
	ParticleSystem particle;

	/* Shaders */
	Shader modelShader, baseObjShader, cubemapShader, terrianShader, grassShader, particleShader;

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
	void transformateModel(const char* objname, float translate[3], float rotation[3], float scale[3]);
	void importAnimation(const char* name, const char* path);
	void exportAnimation(const char* objname, const char* filename, const char* animation);
	void addAnimation(const char* name, AnimationClip clip);
	void deleteAnimation(const char* objname, const char* animation);
	void playAnimation(const char* objName, const char* animation);

	const std::unordered_map<std::string, Model>& getModels() const;
	const Model& getModel(const char* name) const;
};

