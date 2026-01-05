//
// Created by voltairepunk on 12/27/25.
//

#include "ParticleSystem.h"

#include <algorithm>
#include <cmath>
#include <iostream>

#include <SFML/OpenGL.hpp>

namespace atmosil::atmo {

    constexpr const float kDefaultLifetime = 25.0;
    constexpr const float kArbitrarySpeedModifier = 10.f;
    constexpr const float kVelocityDegradeFactor = 0.99f;

    constexpr const char* kShaderVert = "shaders/smoke.vert";
    constexpr const char* kShaderFrag = "shaders/smoke.frag";

    ParticleSystem::ParticleSystem(const int count) {
        positions_.reserve(count);
        velocities_.reserve(count);
        lifetimes_.reserve(count);

        for (int i = 0; i < count; i++) {
            positions_.emplace_back(0.0f, 0.0f);
            velocities_.emplace_back(0.0f, 0.0f);
            lifetimes_.emplace_back(kDefaultLifetime);
        }

        if (!shader_.loadFromFile(kShaderVert, kShaderFrag)) {
            std::cerr << "Failed to load smoke shaders" << std::endl;
        }
    }

    void
    ParticleSystem::UpdateParticles(const sf::Time dt, const Space &space,
                                    const std::function<void(sf::Vector2f &, sf::Vector2f &, float &)> &update_lamda) {
        auto idx = 0;
        for (auto &position: positions_) {
            // if (lifetimes_[idx] <= 0.f) {
            //     continue;
            // }

            auto wind = space.GetWindVelocityAt(position.x, position.y) * 1000.f;

            float lerp_factor = std::min(1.0f, kArbitrarySpeedModifier * dt.asSeconds());
            velocities_[idx] += (wind - velocities_[idx]) * lerp_factor;
            lifetimes_[idx] = space.GetPressureAt(position.x, position.y);

            update_lamda(position, wind, lifetimes_[idx]);

            idx++;
        }
    }
    void ParticleSystem::UpdateSystem(const sf::Time dt) {
        auto idx = 0;

        while (!new_particle_queue_.empty()) {
            auto [position, velocity, lifetime] = new_particle_queue_.front();

            positions_.push_back(position);
            velocities_.push_back(velocity);
            lifetimes_.push_back(lifetime);

            new_particle_queue_.pop();
        }

        for (auto &position: positions_) {
            position += velocities_[idx] * dt.asSeconds();
            velocities_[idx] *= kVelocityDegradeFactor;

            idx++;
        }
    }

    void ParticleSystem::Render(sf::RenderTexture &renderTexture) const {
        sf::VertexArray va(sf::PrimitiveType::Points);
        va.resize(positions_.size());

        sf::RenderStates rs;
        rs.shader = &shader_;
        rs.blendMode = sf::BlendAdd;
        rs.texture = nullptr;

        for (auto idx = 0; idx < positions_.size(); idx++) {
            const auto &position = positions_[idx];

            // Get pointer to the start of this particle's vertices in the array
            sf::Vertex *vertex = &va[idx];
            vertex->position = position;

            // 4. Color/Alpha (Fading out)
            vertex->color = sf::Color::White;
        }

        glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
        glEnable(GL_POINT_SPRITE);

        renderTexture.draw(va, rs);

        glDisable(GL_POINT_SPRITE);
        glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
    }
    void ParticleSystem::AddParticle(const Particle &particle) { new_particle_queue_.push(particle); }
} // namespace atmosil::atmo
