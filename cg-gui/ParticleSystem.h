#pragma once
#include "Definition.h"
#include "Shader.h"

enum ParticleType {
    PARTICLE_TYPE_FOUNTAIN,
    PARTICLE_TYPE_FIRE,
    PARTICLE_TYPE_KAMEHAMEHA,
    PARTICLE_TYPE_NONE
};

struct Particle {
    /* Particle */
    glm::vec3 position;
    glm::vec3 velocity;
    float lifetime, maxLifetime;
    glm::vec4 color;
    float size;
    int trailCount;

    Particle() :
        position(glm::vec3(0.f)), velocity(glm::vec3(0.f)), lifetime(0), maxLifetime(0),
        color(glm::vec4(0.f)), size(0), trailCount(0) {}
};


class ParticleSystem {
private:
    ParticleType type;

    std::vector<Particle> particles;
    float particleLife;
    int maxParticles;
    int lastUsedParticle;

    GLuint VAO, VBO;
    GLuint textureID;
    bool useTexture;
    
    void respawnParticle(Particle& p, const glm::vec3& pos, const glm::vec3& vel);

public:
    ParticleSystem();
    ParticleSystem(ParticleType type, float particleLife, int maxParticles);

    void loadTexuture(const std::string& path);
    void emit(const glm::vec3& pos, const glm::vec3& vel, const int& count);
    void update(float dt);
    void drawPrevVelocity(const Shader& shader);
    void draw(const Shader& shader);
};

