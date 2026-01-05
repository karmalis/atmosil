//
// Created by voltairepunk on 12/26/25.
//

#ifndef ATMOSIL_ATMODEBUGCONTROL_H
#define ATMOSIL_ATMODEBUGCONTROL_H

#include <SFML/Graphics.hpp>

#include "GasRenderer.h"
#include "ParticleSystem.h"
#include "Space.h"

namespace atmosil::atmo {

    class AtmoDebugControl {
    public:

        enum Mode {
            SimMode = 0,
            WallEditMode = 1,
            AddParticle = 2,
        };

        AtmoDebugControl(int width, int height, float tile_size);

        void Update(sf::Time dt);

        void Render(sf::RenderTexture &renderTexture);

        void RenderDebugPanel();

        void UpdateWalls(sf::Vector2f pos);

        void UpdateFlowTexture();

    private:

        bool HandleDebugSimMode();

        GasRenderer renderer_;
        Space space_;
        ParticleSystem particle_system_;

        bool running_;
        Mode mode_;

        struct {
            int control_state{0};
        } debug_ui_state_flags;

        sf::Clock clock_;
        sf::Time last_update_time_{sf::Time::Zero};

        int width_;
        int height_;
        int tile_size_;

        sf::Image flow_image_;
        sf::Texture flow_texture_;

        sf::Shader velocity_shader_;
        sf::Sprite flow_debug_sprite_;
    };

} // namespace atmosil::atmo


#endif // ATMOSIL_ATMODEBUGCONTROL_H
