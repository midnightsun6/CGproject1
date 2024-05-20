#include "ParticleSystem.h"

void ParticleSystem::respawnParticle(Particle& p) {
    p.position = glm::vec3(0.f);
    float angle = glm::radians(static_cast<float>(rand() % 360));
    float speed = 10.0f + static_cast<float>(rand() % 5);
    p.velocity = glm::vec3(cos(angle) * speed, speed, sin(angle) * speed);
    p.lifetime = p.maxLifetime = rand() % 10 + particleLife;
    p.color = glm::vec4(rand() % 256 / 256.f, rand() % 256 / 256.f, rand() % 256 / 256.f, 1.f);
    p.radius = rand() % 5 + 1;
}

ParticleSystem::ParticleSystem() : particleLife(0), maxParticles(0) {}

ParticleSystem::ParticleSystem(float particleLife, int maxParticles)
    : particleLife(particleLife), maxParticles(maxParticles) {
    particles.resize(maxParticles);
    for (Particle& p : particles) {
        respawnParticle(p);
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glGenVertexArrays(1, &trailVAO);
    glGenBuffers(1, &trailVBO);
}

void ParticleSystem::update(float dt) {
    currTime += dt;
    for (auto& p : particles) {
        p.lifetime -= dt;
        
        if (p.lifetime > 0) {
            // Simulate gravity
            p.velocity.y += -9.8f * dt;
            p.position += p.velocity * dt;
        }
        else {
            respawnParticle(p);
        }
    }
}

void ParticleSystem::draw(const Shader& shader) {
    glEnable(GL_BLEND);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_POINT_SPRITE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LEQUAL);

    shader.setUniform("model", glm::mat4(1.f));

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

    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, lifetime));
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, radius));
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(4, 1);

    glDrawArraysInstanced(GL_POINTS, 0, 1, particles.size());
    //glDrawArraysInstanced(GL_POINTS, 0, particles.size(), 1);
}
