#include "MainScene.h"

MainScene::MainScene() {
    totalTime = 0;
}

MainScene::~MainScene() {}

bool MainScene::Initialize() {
    GLenum res = glewInit();
    if (res != GLEW_OK) {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return false;
    }

    glClearColor(0, 0, 0, 1);
    glEnable(GL_DEPTH_TEST);

    // Init objects.
    grid.Init(GL_LINES);
    setGrid();

    box.Init(GL_TRIANGLES);
    setBox(1);

    // addSphere(2.0, 100, 100, glm::vec3(0.2, 0.2, 0.2));

    // Load shaders
    baseObjShader.setShader("shader.vs", "shader.fs");
    modelShader.setShader("model_loading.vs", "model_loading.fs");
    cubemapShader.setShader("cubemap.vs.glsl", "cubemap.fs.glsl");

    return true;
}

void MainScene::Update(double dt) {
    // Update total time for object movement
    totalTime += dt;

    // Update all objects
    camera.update();
    for (auto& [_, m] : models) {
        m.update(dt);
    }

    float angle = 90, speed = 5;
    box.setModel(glm::mat4(1.0f));
    box.Rotate(totalTime * angle, 0, 1, 0);
    box.Translate(totalTime * speed, 0, 0);
}

void MainScene::Render() {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // BaseObjects
    baseObjShader.use();
    box.Draw(camera.getProjectionMatrix(), camera.getViewMatrix(), baseObjShader);
    grid.Draw(camera.getProjectionMatrix(), camera.getViewMatrix(), baseObjShader);
    for (auto& obj : spheres) {
        obj.Draw(camera.getProjectionMatrix(), camera.getViewMatrix(), baseObjShader);
    }

    // Models
    modelShader.use();
    modelShader.setMat4("projection", camera.getProjectionMatrix());
    modelShader.setMat4("view", camera.getViewMatrix());

    // Light
    glm::vec3 lightPos = glm::vec3(-5.0, 10.f, 10.f);
    glm::vec3 lightColor = glm::vec3(1.0f);
    modelShader.setVec3("viewPos", camera.getCameraPos());
    modelShader.setVec3("lightPos", camera.getCameraPos());
    modelShader.setVec3("lightColor", lightColor);

    for (auto& [_, m] : models) {
        m.draw(modelShader);
    }

    // Skybox
    cubemapShader.use();
    cubemapShader.setMat4("projection", camera.getProjectionMatrix());
    cubemapShader.setMat4("view", camera.getViewMatrix());
    skybox.draw(cubemapShader);
}

void MainScene::OnResize(int width, int height) {
    camera.updateWindowSize(width, height);
    std::cout << "MainScene Resize: " << width << " " << height << std::endl;
}

void MainScene::loadModel(const char* path, const char* name) {
    Model model;
    model.setModel(path);
    this->models[name] = model;
}

void MainScene::transformateModel(const char* objname, float translate[3], float rotation[3], float scale[3]) {
    if (models.count(objname)) {
        models[objname].resetMatrix();
        models[objname].transformate(translate, rotation, scale);
    }
}

void MainScene::importAnimation(const char* name, const char* path) {
    if (models.count(name)) {
        models[name].importAnimation(path);
    }
}

void MainScene::exportAnimation(const char* objname, const char* filename, const char* animation) {
    if (models.count(objname)) {
        models[objname].exportAnimtion(filename, animation);
    }
}

void MainScene::addAnimation(const char* name, AnimationClip clip) {
    if (models.count(name)) {
        models[name].addAnimation(clip);
    }
}

void MainScene::deleteAnimation(const char* objname, const char* animation) {
    if (models.count(objname)) {
        models[objname].deleteAnimation(animation);
    }
}

void MainScene::playAnimation(const char* objName, const char* animation) {
    if (models.count(objName)) {
        models[objName].playAnimation(animation);
    }
}

const std::unordered_map<std::string, Model>& MainScene::getModels() const {
    return this->models;
}

const Model& MainScene::getModel(const char* name) const {
    if (models.count(name)) {
        return models.at(name);
    }
    return Model();
}

void MainScene::setBox(float size) {
    // Set points color.
    for (int i = 0; i < 8; i++) {
        std::vector<float> rgb(3);
        for (int j = 0; j < 3; j++) {
            rgb[j] = (rand() % 101) / 100.f;
        }
        box.addColor(glm::vec3(rgb[0], rgb[1], rgb[2]));
    }

    // Set points position.
    box.addPoint(glm::vec3(-size, -size, -size));   // 0
    box.addPoint(glm::vec3(-size, -size, size));    // 1
    box.addPoint(glm::vec3(-size, size, -size));    // 2
    box.addPoint(glm::vec3(-size, size, size));     // 3
    box.addPoint(glm::vec3(size, -size, -size));    // 4
    box.addPoint(glm::vec3(size, -size, size));     // 5
    box.addPoint(glm::vec3(size, size, -size));     // 6
    box.addPoint(glm::vec3(size, size, size));      // 7

    // Set indices.
    /*
            6-------7
           /|      /|
          2-------3 |
          | |     | |
          | 4-----|-5
          |/      |/
          0-------1
    */
    box.addIndex(1); box.addIndex(5); box.addIndex(7);
    box.addIndex(7); box.addIndex(3); box.addIndex(1);

    box.addIndex(5); box.addIndex(4); box.addIndex(6);
    box.addIndex(6); box.addIndex(7); box.addIndex(5);

    box.addIndex(4); box.addIndex(0); box.addIndex(2);
    box.addIndex(2); box.addIndex(6); box.addIndex(4);

    box.addIndex(0); box.addIndex(1); box.addIndex(3);
    box.addIndex(3); box.addIndex(2); box.addIndex(0);

    box.addIndex(3); box.addIndex(7); box.addIndex(6);
    box.addIndex(6); box.addIndex(2); box.addIndex(3);

    box.addIndex(4); box.addIndex(5); box.addIndex(1);
    box.addIndex(1); box.addIndex(0); box.addIndex(4);
}

void MainScene::setGrid() {
    glm::vec3 color = glm::vec3(0.2f, 0.2f, 0.2f);
    int hw = 500, step = 5;
    int count = 0;
    for (int i = -hw; i <= hw; i += step) {
        grid.addPoint(glm::vec3(i, 0, -hw));
        grid.addPoint(glm::vec3(i, 0, hw));
        grid.addPoint(glm::vec3(-hw, 0, i));
        grid.addPoint(glm::vec3(hw, 0, i));
        for (int j = 0; j < 4; j++) {
            grid.addColor(color);
            grid.addIndex(count++);
        }
    }
}

void MainScene::addSphere(float radius, int slices, int stacks, glm::vec3 color) {
    GLBaseObject sphere;
    sphere.Init(GL_TRIANGLE_STRIP);

    for (int i = 0; i <= stacks; i++) {
        float phi = i / (float)stacks * glm::pi<float>();

        for (int j = 0; j <= slices; j++) {
            float theta = j / (float)slices * (glm::pi<float>() * 2);

            float x = cos(theta) * sin(phi) * radius;
            float y = cos(phi) * radius;
            float z = sin(theta) * sin(phi) * radius;

            sphere.addPoint(glm::vec3(x, y, z));
            sphere.addColor(color);
        }
    }

    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < slices; ++j) {
            int first = (i * (slices + 1)) + j;
            int second = first + slices + 1;

            sphere.addIndex(first);
            sphere.addIndex(second);
            sphere.addIndex(first + 1);

            sphere.addIndex(second);
            sphere.addIndex(second + 1);
            sphere.addIndex(first + 1);
        }
    }

    spheres.push_back(sphere);
    std::cout << "Add sphere\n";
}