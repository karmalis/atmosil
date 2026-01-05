//
// Created by voltairepunk on 12/26/25.
//

#include "AtmoDebugControl.h"

#include <iostream>

#include <imgui.h>


namespace atmosil::atmo {

    constexpr const sf::Time kTimePerTick = sf::milliseconds(100);
    constexpr const char* kSmokeVelocityFrag = "shaders/velocity.frag";

    AtmoDebugControl::AtmoDebugControl(const int width, const int height, const float tile_size) :
        renderer_(width, height, tile_size), space_(height, width, tile_size), particle_system_(0), running_(false),
        width_(width), height_(height), tile_size_(tile_size), flow_debug_sprite_(flow_texture_), mode_(SimMode) {

        for (auto y = 0; y < height; y++) {
            for (auto x = 0; x < width; x++) {
                auto half_tile = tile_size / 2.f;
                auto p_pos = sf::Vector2f(static_cast<float>(x) * tile_size + half_tile,
                                          static_cast<float>(y) * tile_size + half_tile);

                ParticleSystem::Particle particle{
                        .position = p_pos, .velocity = sf::Vector2f(0.f, 0.f), .lifetime = 25.f};
                particle_system_.AddParticle(particle);
            }
        }

        if (!velocity_shader_.loadFromFile(kSmokeVelocityFrag, sf::Shader::Type::Fragment)) {
            std::cerr << "Failed to load shader velocity.frag" << std::endl;
        } else {
            velocity_shader_.setUniform("u_flowMap", sf::Shader::CurrentTexture);
        }

        UpdateFlowTexture();
        flow_debug_sprite_.setTexture(flow_texture_, true);
    }

    void AtmoDebugControl::Update(const sf::Time dt) {
        if (running_) {
            last_update_time_ += dt;
            while (last_update_time_ > kTimePerTick) {

                space_.UpdateAtmosphereStep();
                last_update_time_ -= kTimePerTick;
            }
            particle_system_.UpdateParticles(
                    dt, space_, [this](sf::Vector2f &position, sf::Vector2f &wind, float &pressure) {

                    });
        }

        UpdateFlowTexture();
        flow_debug_sprite_.setTexture(flow_texture_, true);

        particle_system_.UpdateSystem(dt);

        curr_pressure_ = space_.GetTotalPressure();
        if (curr_pressure_ > curr_max_pressure_) {
            curr_max_pressure_ = curr_pressure_;
        }
    }

    void AtmoDebugControl::Render(sf::RenderTexture &renderTexture) {
        // renderer_.UpdateGasPressureColor(space_.GetGasPressure());
        // renderer_.Render(renderTexture);


        sf::RenderStates rs;
        rs.shader = &velocity_shader_;

        float scale = static_cast<float>(tile_size_);
        flow_debug_sprite_.setScale(sf::Vector2f(scale, scale));
        renderTexture.draw(flow_debug_sprite_, rs);

        particle_system_.Render(renderTexture);

        // sf::VertexArray wind_lines(sf::PrimitiveType::Lines);
        //
        // for (int y = 0; y < height_; y++) { // You'll need access to width/height here
        //     for (int x = 0; x < width_; x++) {
        //         // Center of the tile
        //         sf::Vector2f center((x * tile_size_) + (tile_size_ / 2.f), (y * tile_size_) + (tile_size_ / 2.f));
        //
        //         // Get wind
        //         sf::Vector2f wind = space_.GetWindVelocityAt(center.x, center.y);
        //
        //         // If wind is tiny, skip
        //         if ((wind.x * wind.x + wind.y * wind.y) < 0.00001f)
        //             continue;
        //
        //         // Scale it up so we can see it (x500 for visualization)
        //         sf::Vector2f end = center + (wind * 500.0f);
        //
        //         wind_lines.append(sf::Vertex(center, sf::Color::Red));
        //         wind_lines.append(sf::Vertex(end, sf::Color::Red));
        //     }
        // }
        // renderTexture.draw(wind_lines);
    }

    void AtmoDebugControl::RenderDebugPanel() {
        auto should_update = false;

        ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_FirstUseEver);
        ImGui::Begin("Space Atmo Controls");

        ImGui::Text("Mode:");
        ImGui::RadioButton("Simulation mode", &debug_ui_state_flags.control_state, SimMode);
        ImGui::RadioButton("Wall edit mode", &debug_ui_state_flags.control_state, WallEditMode);
        ImGui::RadioButton("Add particle mode", &debug_ui_state_flags.control_state, AddParticle);

        ImGui::InputFloat("Max Pressure", &curr_max_pressure_);
        ImGui::InputFloat("Curr Pressure", &curr_pressure_);

        switch (debug_ui_state_flags.control_state) {
            case SimMode: {
                should_update = HandleDebugSimMode();
            } break;
            case WallEditMode: {

            } break;
            default: {
            } break;
        }


        if (should_update) {
            space_.UpdateAtmosphereStep();
        }

        ImGui::End();
    }
    void AtmoDebugControl::UpdateWalls(sf::Vector2f p) {
        if (debug_ui_state_flags.control_state == WallEditMode) {
            const auto pos = renderer_.PixelToGrid(p);
            space_.AddWall(pos.x, pos.y);
        } else if (debug_ui_state_flags.control_state == AddParticle) {
            ParticleSystem::Particle particle{.position = p, .velocity = sf::Vector2f(0.f, 0.f), .lifetime = 25.f};
            particle_system_.AddParticle(particle);
        }
    }

    bool AtmoDebugControl::HandleDebugSimMode() {
        auto should_update = false;

        if (ImGui::Button("Single Sim Step")) {
            should_update = true;
        }

        if (ImGui::Button("Run Sim")) {
            running_ = true;
            should_update = false;
        }

        if (ImGui::Button("Stop Sim")) {
            running_ = false;
            should_update = false;
        }

        return should_update;
    }

    void AtmoDebugControl::UpdateFlowTexture() {
        if (flow_image_.getSize() != sf::Vector2u(width_, height_)) {
            flow_image_.resize(sf::Vector2u(width_, height_), sf::Color(128, 128, 0));
        }

        for (unsigned y = 0; y < height_; ++y) {
            for (unsigned x = 0; x < width_; ++x) {
                const auto idx = y * width_ + x;

                // --- FIX: Convert Grid Index -> World Coordinate ---
                // Get center of the tile to ensure accurate sampling
                float worldX = (static_cast<float>(x) * tile_size_) + (tile_size_ / 2.0f);
                float worldY = (static_cast<float>(y) * tile_size_) + (tile_size_ / 2.0f);

                // Now pass World Coordinates
                const auto wind = space_.GetWindVelocityAt(worldX, worldY);
                const auto pressure = space_.GetPressureAt(worldX, worldY);

                // --- FIX 1: Tame the Wind ---
                // Reduce multiplier from 10.0 to 2.0.
                // Now wind needs to be 0.5 magnitude to hit max color.
                float r_f = (wind.x * 2.0f * 127.5f) + 127.5f;
                float g_f = (wind.y * 2.0f * 127.5f) + 127.5f;

                // --- FIX 2: Tame the Pressure ---
                // Instead of pressure * 255, let's assume 1.0 is "normal" (50% brightness).
                // This leaves room for high pressure (up to 2.0) before maxing out.
                float b_f = pressure * 100.0f;

                uint8_t r = static_cast<uint8_t>(std::clamp(r_f, 0.f, 255.f));
                uint8_t g = static_cast<uint8_t>(std::clamp(g_f, 0.f, 255.f));
                uint8_t b = static_cast<uint8_t>(std::clamp(b_f, 0.f, 255.f));

                flow_image_.setPixel(sf::Vector2u(x, y), sf::Color(r, g, b));
            }
        }

        // Upload CPU image to GPU Texture
        // Check if texture needs resizing too
        if (flow_texture_.getSize() != flow_image_.getSize()) {
            if (!flow_texture_.resize(flow_image_.getSize())) {
                std::cerr << "AtmoDebugControl::UpdateFlowTexture: flow_texture_.size()" << std::endl;
            }
        }
        flow_texture_.update(flow_image_);
    }
} // namespace atmosil::atmo
