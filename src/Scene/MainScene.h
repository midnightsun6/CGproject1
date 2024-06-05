#pragma once

#include "../../cg-gui/Model.h"
#include "../../cg-gui/Camera.h"
#include "../../cg-gui/GLBaseObject.h"
#include "../../cg-gui/Skybox.h"
#include "../../cg-gui/Terrian.h"
#include "../../cg-gui/Grass.h"
#include "../../cg-gui/ParticleSystem.h"
#include "../../cg-gui/ShaderManager.h"
#include "../../cg-gui/ReflectionSphere.h"
#include "../../cg-gui/MotionBlur.h"
#include "../../cg-gui/Mosaic.h"
#include "../../cg-gui/Water.h"
#include "../../cg-gui/PointLightCube.h"

enum RenderSceneParticle {
	RENDER_SCENE_NORMAL,
	RENDER_SCENE_TOON,
	RENDER_SCENE_MOSAIC,
	RENDER_SCENE_MOTION_BLUR,
};

enum RenderParticle {
	RENDER_PARTICLE_KAMEHAMEHA,
	RENDER_PARTICLE_FIRE,
	RENDER_PARTICLE_NONE
};

enum RenderReflection {
	RENDER_REFLECTION_MIRROR,
	RENDER_REFLECTION_WATER,
	RENDER_REFELCTION_NONE,
};

class MainScene {
private:
	float totalTime;

	int screenWidth = 1280;
	int screenHeight = 760;

	RenderSceneParticle renderSceneParticle;
	RenderParticle renderParticle;
	RenderReflection renderReflection;

	Camera camera;

	/* Background */
	Skybox skybox;
	Terrian terrian;
	Grass grass;

	/* Base Objects (temp) */
	GLBaseObject grid, box;
	std::vector<GLBaseObject> spheres;

	/* Object */
	Water water;
	ReflectionSphere mirror;
	std::unordered_map<std::string, Model> models;

	/* Particle */
	ParticleSystem particle;
	MotionBlur motionBlur;
	Mosaic mosaic;
	PointLightCube lightCube;

	/* Shaders */
	ShaderManager* shaderManager = ShaderManager::getInstance();

public:
	MainScene();
	~MainScene();

	/*
		GLFWkey function cannot use the reference lambda function => cannot access InputController in App.h.
		Temporary sol: Set InputController as public.
	*/
	InputController* input = InputController::getInstance();

	/* Scene functions */
	bool initialize();
	void update(double dt);
	void render();

	/* Detail Render Functions */
	void captureEnvironment();
	void renderScene(const glm::mat4& projection, const glm::mat4& view, const int& screenWidth, const int& screenHeight);
	void renderWaterReflection();
	void renderReflectionSphere();
	void renderWater();
	void renderVelocity();
	void renderColor();
	void renderMotionBlur();
	void renderMosaic();
	void renderShadowMap();

	/* Callback functions */
	void onResize(int width, int height);

	/* OpenGL Base Object functions */
	void setBox(float size);
	void setGrid();
	void addSphere(float radius, int slices, int stacks, glm::vec3 color);

	/* OpenGL Model functions */
	void setRenderSceneParticle(RenderSceneParticle rs);
	void setRenderParticle(RenderParticle rp);
	void setRenderReflection(RenderReflection rr);

	void setModelAmount(int amount);
	void setMosaicBlockSize(float blockSize);
	void setBlurScale(float blurscale);

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

