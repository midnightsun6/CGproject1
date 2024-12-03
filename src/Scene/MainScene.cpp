#include "MainScene.h"

bool MainScene::isRenderWater()
{
    return this->renderReflection == RENDER_REFLECTION_WATER_SIN_WAVE 
    || this->renderReflection == RENDER_REFLECTION_WATER_HEIGHT_MAP;
}

MainScene::MainScene()
{
    totalTime = 0;
}

MainScene::~MainScene() {}

bool MainScene::initialize() {
    GLenum res = glewInit();
    if (res != GLEW_OK) {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return false;
    }

    shaderManager->init();

    this->renderSceneParticle = RENDER_SCENE_NORMAL;
    this->renderParticle = RENDER_PARTICLE_NONE;
    this->renderReflection = RENDER_REFELCTION_NONE;

    glClearColor(0, 0, 0, 1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    // Init objects.
    grid.init(GL_LINES);
    setGrid();

    box.init(GL_TRIANGLES);
    setBox(1);

    mirror.init(5.0, 100, 100);
    mirror.translate(0, 30, 0);
    lightCube.setPosition(glm::vec3(0.f, 50.f, 10.f));

    //addSphere(2.0, 100, 100, glm::vec3(1.0, 0.2, 0.2));

    //particle = ParticleSystem(PARTICLE_TYPE_FOUNTAIN, 10.f, 50000);
    //particle = ParticleSystem(PARTICLE_TYPE_KAMEHAMEHA, 5, 1e5);
    //particle.loadTexuture("kamehameha1.png");

    ///* Loading model and animations */
    std::cout << "Loading Model: AndroidBot.obj...\n";
    this->loadModel("Android Robot/AndroidBot.obj", "Android");
    std::cout << "Loading Successfully\n";

    //std::cout << "Loading Animation: Kamehameha.objani...\n";
    //this->importAnimation("Android", "Kamehameha.objani");
    //std::cout << "Loading Successfully\n";

    //std::cout << "Loading Animation: Buu.objani...\n";
    //this->importAnimation("Android", "Buu.objani");
    //std::cout << "Loading Successfully\n";

    //std::cout << "Loading Animation: Empty.objani...\n";
    //this->importAnimation("Android", "Empty.objani");
    //std::cout << "Loading Successfully\n";

    ///* Loading floor */
    //std::cout << "Loading Model: Floor.obj...\n";
    //this->loadModel("floor/Floor.obj", "Floor");
    //std::cout << "Loading Successfully\n";

    models["Android"].translate(0.f, -10.f, 0.f);
    /*models["Floor"].translate(0.f, 10.f, 0.f);
    models["Floor"].scale(5.f, 1.f, 5.f);*/

    water.setWindowSize(this->screenWidth, this->screenHeight);
    motionBlur.setScreenSize(this->screenWidth, this->screenHeight);
    mosaic.setScreenSize(this->screenWidth, this->screenHeight);

    return true;
}

void MainScene::update(double dt) {
    // Update total time for object movement
    totalTime += dt;

    // Update all objects
    camera.update();
    for (auto& [_, m] : models) {
        m.update(dt);
    }

    float angle = 90, speed = 5;
    box.setModel(glm::mat4(1.0f));
    box.rotate(totalTime * angle, 0, 1, 0);
    box.translate(totalTime * speed, 0, 0);

    water.update(dt);

    /*particle.emit(glm::vec3(0, 10, 0), glm::vec3(20, 0, 0), 1000);
    particle.update(dt);*/

    if (input->getKeyPress('Z')) {
        glm::vec3 pos = camera.getCameraFront();
        pos *= 3;
        pos += camera.getCameraPos();
        //pos.y -= 3.0f;
        lightCube.setPosition(pos);
    }
}

void MainScene::render() {
    // 1. Render reflection object of reflection texture
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, this->screenWidth, this->screenHeight);

    switch (this->renderReflection) {
        case RENDER_REFLECTION_MIRROR:
        {
            this->captureEnvironment();
            break;
        }
        case RENDER_REFLECTION_WATER_SIN_WAVE: case RENDER_REFLECTION_WATER_HEIGHT_MAP:
        {
            this->renderWaterReflection();
            this->renderWaterRefraction();
            break;
        }
        default:
            break;
    }

    // 2. Render scene with scene particle
    switch (this->renderSceneParticle) {
        case RENDER_SCENE_NORMAL: case RENDER_SCENE_TOON:
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            this->renderShadowMap();

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            this->renderScene(camera.getProjectionMatrix(), camera.getViewMatrix(), this->screenWidth, this->screenHeight);
            break;
        }
        case RENDER_SCENE_MOSAIC:
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            this->renderMosaic();
            break;
        }
        case RENDER_SCENE_MOTION_BLUR:
        {
            this->renderScene(camera.getProjectionMatrix(), camera.getViewMatrix(), this->screenWidth, this->screenHeight);
            this->renderScene(camera.getProjectionMatrix(), camera.getViewMatrix(), this->screenWidth, this->screenHeight);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            this->renderVelocity();
            this->renderColor();
            this->renderMotionBlur();
            break;
        }
        default:
            break;
    }

    // 3. Render reflection object
    switch (this->renderReflection) {
        case RENDER_REFLECTION_MIRROR:
        {
            this->renderReflectionSphere();
            break;
        }
        case RENDER_REFLECTION_WATER_SIN_WAVE: case RENDER_REFLECTION_WATER_HEIGHT_MAP:
        {
            this->renderWater();
            break;
        }
        default:
            break;
    }
}

void MainScene::captureEnvironment() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const GLuint& captureFBO = mirror.getCaptureFBO();
    const GLuint& captureRBO = mirror.getCaptureRBO();
    const GLuint& envCubemap = mirror.getEnvCubemap();
    const int& captureSize = mirror.getCaptureSize();

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, captureSize, captureSize);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    for (unsigned int i = 0; i < 6; ++i) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, captureSize, captureSize);

        this->renderScene(mirror.getCaptureProjection(), mirror.getCaptureView(i), captureSize, captureSize);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}

void MainScene::renderScene(const glm::mat4& projection, const glm::mat4& view, const int& screenWidth, const int& screenHeight, const glm::vec4 clipPlane) {
    // Skybox
    shaderManager->useShader(SHADER_CUBEMAP);
    shaderManager->setCurrUniform("projection", projection);
    shaderManager->setCurrUniform("view", view);
    shaderManager->setCurrUniform("clipPlane", clipPlane);
    skybox.draw(shaderManager->getCurrShader());

    // Terrian
    if (!isRenderWater()) {
        shaderManager->useShader(SHADER_TERRIAN);
        shaderManager->setCurrUniform("projection", projection);
        shaderManager->setCurrUniform("view", view);
        terrian.draw(shaderManager->getCurrShader());
    }

    // Grass
    shaderManager->useShader(SHADER_GRASS);
    shaderManager->setCurrUniform("projection", projection);
    shaderManager->setCurrUniform("view", view);
    shaderManager->setCurrUniform("clipPlane", clipPlane);
    grass.draw(shaderManager->getCurrShader());

    // Model
    if (this->renderSceneParticle == RENDER_SCENE_TOON) {
        shaderManager->useShader(SHADER_MODEL_TOON);
    }
    else {
        shaderManager->useShader(SHADER_MODEL_SHADOW);
    }
    shaderManager->setCurrUniform("projection", projection);
    shaderManager->setCurrUniform("view", view);
    shaderManager->setCurrUniform("clipPlane", clipPlane);

    shaderManager->setCurrUniform("viewPos", camera.getCameraPos());
    shaderManager->setCurrUniform("lightPos", lightCube.getPosition());
    shaderManager->setCurrUniform("lightColor", lightCube.getColor());
    shaderManager->setCurrUniform("farPlane", lightCube.getFarPlane());

    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_CUBE_MAP, lightCube.getDepthCubemap());
    shaderManager->setCurrUniform("shadowMap", 10);
    for (auto& [_, m] : models) {
        m.draw(shaderManager->getCurrShader());
    }

    // Particle
    shaderManager->useShader(SHADER_PARTICLE);
    shaderManager->setCurrUniform("projection", projection);
    shaderManager->setCurrUniform("view", view);
    //particle.draw(shaderManager->getCurrShader());
    for (auto& [_, m] : models) {
        m.drawParticle(shaderManager->getCurrShader());
    }

    // BaseObjects
    shaderManager->useShader(SHADER_BASE_SHAPE);
    shaderManager->setCurrUniform("projection", projection);
    shaderManager->setCurrUniform("view", view);
    //box.draw(shaderManager->getCurrShader());
    //grid.draw(shaderManager->getCurrShader());
    for (auto& obj : spheres) {
        //obj.draw(shaderManager->getCurrShader());
    }

    shaderManager->useShader(SHADER_LIGHT_CUBE);
    shaderManager->setCurrUniform("projection", projection);
    shaderManager->setCurrUniform("view", view);
    lightCube.draw(shaderManager->getCurrShader());
}

void MainScene::renderWaterReflection() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, water.getReflectionFBO());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float waterHeight = 0.f;
    glm::vec3 reflectPos = camera.getCameraPos();
    reflectPos.y = 2 * waterHeight - camera.getCameraPos().y;
    glm::vec3 reflectDir = camera.getCameraFront();
    reflectDir.y *= -1;
    glm::vec3 reflectUp = glm::vec3(0.f, 1.f, 0.f);
    glm::mat4 reflectView = glm::lookAt(reflectPos, reflectPos + reflectDir, reflectUp);
    this->renderScene(camera.getProjectionMatrix(), reflectView, this->screenWidth, this->screenHeight);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MainScene::renderWaterRefraction() {
    glEnable(GL_CLIP_DISTANCE0);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, water.getRefractionFBO());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float waterHeight = 0.0f;
    glm::vec4 clipPlane(0.0f, -1.0f, 0.0f, waterHeight);
    this->renderScene(camera.getProjectionMatrix(), camera.getViewMatrix(), this->screenWidth, this->screenHeight,clipPlane);

    glDisable(GL_CLIP_DISTANCE0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MainScene::renderReflectionSphere() {
    // Reflection Object
    shaderManager->useShader(SHADER_REFLECTION);
    shaderManager->setCurrUniform("projection", camera.getProjectionMatrix());
    shaderManager->setCurrUniform("view", camera.getViewMatrix());
    shaderManager->setCurrUniform("cameraPos", camera.getCameraPos());
    mirror.draw(shaderManager->getCurrShader());
}

void MainScene::renderWater() {
    // Water
    shaderManager->useShader(SHADER_WATER);
    shaderManager->setCurrUniform("projection", camera.getProjectionMatrix());
    shaderManager->setCurrUniform("view", camera.getViewMatrix());
    shaderManager->setCurrUniform("viewPos", camera.getCameraPos());
    shaderManager->setCurrUniform("lightPos", lightCube.getPosition());
    shaderManager->setCurrUniform("lightColor", lightCube.getColor());
    shaderManager->setCurrUniform("isHeightMap", (bool)(this->renderReflection == RENDER_REFLECTION_WATER_HEIGHT_MAP));
    water.draw(shaderManager->getCurrShader(), skybox.getSkyboxTexture());
}

void MainScene::renderVelocity() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, motionBlur.getVelocityFBO());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, screenWidth, screenHeight);

    glm::mat4 projection = camera.getProjectionMatrix();
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 prevProjection = camera.getPrevProjectionMatrix();
    glm::mat4 prevView = camera.getPrevViewMatrix();

    // Velocity shader
    shaderManager->useShader(SHADER_VELOCITY);
    shaderManager->setCurrUniform("projection", projection);
    shaderManager->setCurrUniform("view", view);
    shaderManager->setCurrUniform("prevProjection", prevProjection);
    shaderManager->setCurrUniform("prevView", prevView);
    skybox.drawPrevVelocity(shaderManager->getCurrShader());
    terrian.drawPrevVelocity(shaderManager->getCurrShader());
    grass.drawPrevVelocity(shaderManager->getCurrShader());
    for (auto& [_, m] : models) {
        m.drawPrevVelocity(shaderManager->getCurrShader());
    }

    // Particle velocity shader
    shaderManager->useShader(SHADER_VELOCITY_PARTICLE);
    shaderManager->setCurrUniform("projection", projection);
    shaderManager->setCurrUniform("view", view);
    shaderManager->setCurrUniform("prevProjection", prevProjection);
    shaderManager->setCurrUniform("prevView", prevView);
    //particle.drawPrevVelocity(shaderManager->getCurrShader());
    for (auto& [_, m] : models) {
        m.drawParticlePrevVeloctiy(shaderManager->getCurrShader());
    }

    // Model


    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MainScene::renderColor() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, motionBlur.getColorFBO());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->renderScene(camera.getProjectionMatrix(), camera.getViewMatrix(), this->screenWidth, this->screenHeight);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MainScene::renderMotionBlur() {
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderManager->useShader(SHADER_MONTION_BULR);
    motionBlur.renderMotionBlur(shaderManager->getCurrShader());
}

void MainScene::renderMosaic() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, mosaic.getMosaicFBO());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->renderScene(camera.getProjectionMatrix(), camera.getViewMatrix(), this->screenWidth, this->screenHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    shaderManager->useShader(SHADER_MOSAIC);
    mosaic.renderMosaic(shaderManager->getCurrShader());
}

void MainScene::renderShadowMap() {
    glViewport(0, 0, lightCube.getShadowWidth(), lightCube.getShadowHeight());
    glBindFramebuffer(GL_FRAMEBUFFER, lightCube.getDepthMapFBO());
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    std::vector<glm::mat4> shadowViews = lightCube.getViewMatrices();

    shaderManager->useShader(SHADER_DEPTH_CUBEMAP);
    shaderManager->setCurrUniform("lightPos", lightCube.getPosition());
    shaderManager->setCurrUniform("farPlane", lightCube.getFarPlane());
    for (unsigned int i = 0; i < shadowViews.size(); ++i) {
        shaderManager->setCurrUniform("shadowMatrices[" + std::to_string(i) + "]", shadowViews[i]);
    }
    

    // Model's shadow depth map
    for (auto& [_, m] : models) {
        m.drawDepthMap(shaderManager->getCurrShader());
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, this->screenWidth, this->screenHeight);
}

void MainScene::onResize(int width, int height) {
    this->screenWidth = width;
    this->screenHeight = height;
    camera.updateWindowSize(width, height);
    water.setWindowSize(this->screenWidth, this->screenHeight);
    motionBlur.setScreenSize(this->screenWidth, this->screenHeight);
    mosaic.setScreenSize(this->screenWidth, this->screenHeight);
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
    sphere.init(GL_TRIANGLE_STRIP);

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

void MainScene::setRenderSceneParticle(RenderSceneParticle rs) {
    this->renderSceneParticle = rs;
}

void MainScene::setRenderParticle(RenderParticle rp) {
    this->renderParticle = rp;

    switch (renderParticle) {
        case RENDER_PARTICLE_KAMEHAMEHA:
            this->playAnimation("Android", "Kamehameha");
            break;
        case RENDER_PARTICLE_FIRE:
            this->playAnimation("Android", "Buu");
            break;
        case RENDER_PARTICLE_NONE:
            this->playAnimation("Android", "Empty");
            break;
        default:
            break;
    }
}

void MainScene::setRenderReflection(RenderReflection rr) {
    this->renderReflection = rr;
}

void MainScene::setModelAmount(int amount) {
    if (models.count("Android")) {
        models["Android"].setModelAmount(amount);
    }
}

void MainScene::setMosaicBlockSize(float blockSize) {
    this->mosaic.setBlockSize(blockSize);
}

void MainScene::setBlurScale(float blurscale) {
    this->motionBlur.setBlurScale(blurscale);
}
