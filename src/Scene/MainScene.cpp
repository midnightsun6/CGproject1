#include "MainScene.h"

MainScene::MainScene() {

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
    grid.setShader("./res/shaders/shader.vs", "./res/shaders/shader.fs");
    setGrid();

    box.Init(GL_TRIANGLES);
    box.setShader("./res/shaders/shader.vs", "./res/shaders/shader.fs");
    setBox(1);

    // addSphere(2.0, 100, 100, glm::vec3(0.2, 0.2, 0.2));

    model.setModel("./res/models/nanosuit/nanosuit.obj");
    modelShader.setShader("./res/shaders/model_loading.vs", "./res/shaders/model_loading.fs");

    return true;
}

void MainScene::Update(double dt) {
    totalTime += dt;
    camera.Update();

    float angle = 90, speed = 5;
    box.setModel(glm::mat4(1.0f));
    box.Rotate(totalTime * angle, 0, 1, 0);
    box.Translate(totalTime * speed, 0, 0);
}

void MainScene::Render() {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    box.Render(camera.getProjectionMatrix(), camera.getViewMatrix());
    grid.Render(camera.getProjectionMatrix(), camera.getViewMatrix());
    for (auto& obj : spheres) {
        obj.Render(camera.getProjectionMatrix(), camera.getViewMatrix());
    }

    modelShader.use();
    glm::mat4 modelMatrix(1.0f);
    /*modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.75f, 0.0f)) * modelMatrix;
    modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, 0.2f)) * modelMatrix;*/
    glUniformMatrix4fv(glGetUniformLocation(modelShader.getProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(camera.getProjectionMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(modelShader.getProgram(), "view"), 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(modelShader.getProgram(), "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
    model.draw(modelShader);
}

void MainScene::OnResize(int width, int height) {
    camera.UpdateWindowSize(width, height);
    std::cout << "MainScene Resize: " << width << " " << height << std::endl;
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
    sphere.setShader("../../cg-gui/Shaders/shader.vs", "../../cg-gui/Shaders/shader.fs");

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