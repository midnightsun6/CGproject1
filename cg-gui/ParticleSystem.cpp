#include "ParticleSystem.h"

void ParticleSystem::respawnParticle(Particle& p, const glm::vec3& pos, const glm::vec3& vel) {
    auto randomInUnitSphere = []() {
        float theta = static_cast<float>(rand()) / RAND_MAX * 2.0f * glm::pi<float>();
        float phi = static_cast<float>(rand()) / RAND_MAX * glm::pi<float>();
        float r = static_cast<float>(rand()) / RAND_MAX;

        float x = r * sin(phi) * cos(theta);
        float y = r * sin(phi) * sin(theta);
        float z = r * cos(phi);

        return glm::vec3(x, y, z);
    };

    glm::vec3 offset = randomInUnitSphere();

    switch (type) {
        case PARTICLE_TYPE_FOUNTAIN:
        {
            p.position = glm::vec3(0.f);
            float angle = glm::radians(static_cast<float>(rand() % 360));
            float speed = 10.0f + static_cast<float>(rand() % 5);
            p.velocity = glm::vec3(cos(angle) * speed, speed, sin(angle) * speed);
            p.lifetime = p.maxLifetime = rand() % 10 + particleLife;
            p.color = glm::vec4(rand() % 256 / 256.f, rand() % 256 / 256.f, rand() % 256 / 256.f, 1.f);
            p.size = rand() % 5 + 1;
            p.trailCount = 10;
            break;
        }
        case PARTICLE_TYPE_FIRE:
        {
            p.position = pos + offset;
            p.velocity = vel;
            p.lifetime = p.maxLifetime = particleLife;
            p.color = glm::vec4(rand() % 256 / 256.f, 0, 0, 1.0);
            p.size = rand() % 3 + 1;
            p.trailCount = 2;
            break;
        }
        case PARTICLE_TYPE_KAMEHAMEHA:
        {
            p.position = pos + offset;
            p.velocity = vel;
            p.lifetime = p.maxLifetime = particleLife;
            float r = 135.0f + static_cast<float>(rand() % 40);
            float g = 206.0f + static_cast<float>(rand() % 40);
            float b = 250.0f;
            p.color = glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
            p.size = rand() % 3 + 1;
            p.trailCount = 2;
            break;
        }
        default:
            break;
    }
}

ParticleSystem::ParticleSystem() : type(PARTICLE_TYPE_NONE), particleLife(0), maxParticles(0) {}

ParticleSystem::ParticleSystem(ParticleType type, float particleLife, int maxParticles)
    : type(type), particleLife(particleLife), maxParticles(maxParticles) {
    particles.reserve(maxParticles);
    lastUsedParticle = 0;
    useTexture = false;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
}

void ParticleSystem::loadTexuture(const std::string& path) {
    useTexture = true;
    std::string newPath = "./res/particle/" + path;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(newPath.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glGenTextures(1, &textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else {
        std::cout << "Failed to load texture: " << newPath << std::endl;
        useTexture = false;
        return;
    }
    stbi_image_free(data);
}

void ParticleSystem::emit(const glm::vec3& pos, const glm::vec3& vel, const int& count) {
    for (int i = 0; i < count; i++) {
        if (particles.size() < maxParticles) {
            particles.push_back(Particle());
            this->respawnParticle(particles.back(), pos, vel);
        }
        else if (particles[lastUsedParticle].lifetime <= 0.f) {
            this->respawnParticle(particles[lastUsedParticle], pos, vel);
            lastUsedParticle = (lastUsedParticle + 1) % particles.size();
        }
        else break;
    }
}

void ParticleSystem::update(float dt) {
    for (auto& p : particles) {
        p.lifetime -= dt;
        
        if (p.lifetime > 0.f) {
            p.position += p.velocity * dt;

            float lifeRatio = p.lifetime / p.maxLifetime;
            // Simulate gravity
            if (type == PARTICLE_TYPE_FOUNTAIN) {
                p.velocity.y += -9.8f * dt;
            }
            else if (type == PARTICLE_TYPE_KAMEHAMEHA) {
                p.color.a = (1 - lifeRatio) + 0.5;
                p.size = 0.5f / lifeRatio;
            }
        }
    }
}

void ParticleSystem::drawPrevVelocity(const Shader& shader) {
    glEnable(GL_BLEND);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_POINT_SPRITE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LEQUAL);

    shader.setUniform("model", glm::mat4(1.f));
    shader.setUniform("prevModel", glm::mat4(1.f));

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), particles.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, position));
    glEnableVertexAttribArray(0);
    glVertexAttribDivisor(0, 1);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, color));
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, velocity));
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, lifetime));
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, size));
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(4, 1);

    glVertexAttribIPointer(5, 1, GL_INT, sizeof(Particle), (void*)offsetof(Particle, trailCount));
    glEnableVertexAttribArray(5);
    glVertexAttribDivisor(5, 1);

    glDrawArraysInstanced(GL_POINTS, 0, 1, particles.size());

    glDisable(GL_PROGRAM_POINT_SIZE);
    glDisable(GL_BLEND);
}

void ParticleSystem::draw(const Shader& shader) {
    glEnable(GL_BLEND);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_POINT_SPRITE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LEQUAL);

    shader.setUniform("model", glm::mat4(1.f));
    shader.setUniform("useTexture", useTexture);

    if (useTexture) {
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, textureID);
        shader.setUniform("particleTexture", 6);
    }

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), particles.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, position));
    glEnableVertexAttribArray(0);
    glVertexAttribDivisor(0, 1);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, color));
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, velocity));
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, lifetime));
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, size));
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(4, 1);

    //glVertexAttribPointer(5, 1, GL_INT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, trailCount));
    glVertexAttribIPointer(5, 1, GL_INT, sizeof(Particle), (void*)offsetof(Particle, trailCount));
    glEnableVertexAttribArray(5);
    glVertexAttribDivisor(5, 1);

    glDrawArraysInstanced(GL_POINTS, 0, 1, particles.size());

    glDisable(GL_PROGRAM_POINT_SIZE);
    glDisable(GL_BLEND);
}
