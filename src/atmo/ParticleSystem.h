//
// Created by voltairepunk on 12/27/25.
//

#ifndef ATMOSIL_PARTICLESYSTEM_H
#define ATMOSIL_PARTICLESYSTEM_H

#include <functional>
#include <vector>
#include <queue>

#include <SFML/Graphics.hpp>

#include "Space.h"

namespace atmosil::atmo {

    class ParticleSystem {
    public:

        struct Particle {
            sf::Vector2f position;
            sf::Vector2f velocity;
            float lifetime;
        };

        ParticleSystem() = delete;

        explicit ParticleSystem(int count);

        void UpdateParticles(sf::Time dt, const Space &space,
                             const std::function<void(sf::Vector2f&, sf::Vector2f&, float &)> &update_lamda);
        void UpdateSystem(sf::Time dt);

        void Render(sf::RenderTexture &renderTexture) const;

        void AddParticle(const Particle &particle);

    private:
        std::vector<sf::Vector2f> positions_;
        std::vector<sf::Vector2f> velocities_;
        std::vector<float> lifetimes_;

        std::queue<Particle> new_particle_queue_;

        sf::Shader shader_;
    };

} // namespace atmosil::atmo

#endif // ATMOSIL_PARTICLESYSTEM_H
