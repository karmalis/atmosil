//
// Created by voltairepunk on 12/26/25.
//

#include "GasRenderer.h"

constexpr const float kDefaultPadding = 2.0f;

namespace atmosil::atmo {
    sf::Color GetPressureColor(const float pressure) {
        auto const press = (pressure * 255.0);
        if (press <= 0) return {35, 35, 35};       // Vacuum (Grey)
        if (press < 50) return {0, 0, 255};     // Low (Blue)
        if (press < 120) return {0, 255, 0};    // Safe (Green)
        return {255, 0, 0};                        // High (Red)
    }

    GasRenderer::GasRenderer(const int width, const int height, const float tile_size) :
        width_(width), height_(height), tile_size_(tile_size)
    {
        vertex_array_.setPrimitiveType(sf::PrimitiveType::Triangles);
        vertex_array_.resize(width_ * height_ * 6);

        float visible_size = tile_size - (kDefaultPadding * 2.f);

        for (auto y = 0; y < height_; y++) {
            for (auto x = 0; x < width_; x++) {
                const auto idx = (y * width_ + x) * 6;
                const auto px = static_cast<float>(x) * tile_size_;
                const auto py = static_cast<float>(y) * tile_size_;

                const sf::Vector2f top_left{px, py};
                const sf::Vector2f top_right{px + visible_size, py};
                const sf::Vector2f bottom_right{px + visible_size, py + visible_size};
                const sf::Vector2f bottom_left{px, py + visible_size};

                vertex_array_[idx].position = top_left;
                vertex_array_[idx + 1].position = top_right;
                vertex_array_[idx + 2].position = bottom_left;

                vertex_array_[idx + 3].position = top_right;
                vertex_array_[idx + 4].position = bottom_right;
                vertex_array_[idx + 5].position = bottom_left;
            }
        }
    }

    void GasRenderer::UpdateGasPressureColor(const std::vector<float> &pressure) {
        for (auto idx = 0; idx < pressure.size(); idx++) {
            const auto color = GetPressureColor(pressure[idx]);
            const auto v_idx = idx * 6;

            for (auto k = 0; k < 6; k++) {
                vertex_array_[v_idx + k].color = color;
            }
        }
    }
    void GasRenderer::Render(sf::RenderTexture &renderTexture) const {
        renderTexture.draw(vertex_array_);
    }

    sf::Vector2i GasRenderer::PixelToGrid(const sf::Vector2f pixel) const {
        return {
        static_cast<int>(static_cast<float>(pixel.x) / tile_size_),
        static_cast<int>(static_cast<float>(pixel.y) / tile_size_)
        };
    }

} // namespace atmosil::atmo
