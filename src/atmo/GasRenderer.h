//
// Created by voltairepunk on 12/26/25.
//

#ifndef ATMOSIL_SPACERENDERABLE_H
#define ATMOSIL_SPACERENDERABLE_H

#include <SFML/Graphics.hpp>

namespace atmosil::atmo {

    class GasRenderer {
    public:

        GasRenderer() = delete;

        GasRenderer(const int width, const int height, const float tile_size);

        void UpdateGasPressureColor(const std::vector<float>& pressure);

        void Render(sf::RenderTexture& renderTexture) const;

        [[nodiscard]] sf::Vector2i PixelToGrid(sf::Vector2f pixel) const;

    private:

        sf::VertexArray vertex_array_;
        int width_;
        int height_;
        float tile_size_;

    };

} // namespace atmosil::atmo


#endif // ATMOSIL_SPACERENDERABLE_H
