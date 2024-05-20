#pragma once
#include "Definition.h"
#include "Shader.h"


struct Particle {
    /* Particle */
    glm::vec3 position;
    glm::vec3 velocity;
    float lifetime, maxLifetime;
    glm::vec4 color;
    float radius;

    Particle() : position(0.f), velocity(0.f), lifetime(0.f), maxLifetime(0.f), color(glm::vec4(1.f)), radius(5.f) {}
};


class ParticleSystem {
private:
    std::vector<Particle> particles;
    float particleLife;
    int maxParticles;

    float currTime = 0;

    GLuint VAO, VBO;
    GLuint trailVAO, trailVBO;

    void respawnParticle(Particle& p);

public:
    ParticleSystem();
    ParticleSystem(float particleLife, int maxParticles);

    void update(float dt);
    void draw(const Shader& shader);
};

