//
// Created by voltairepunk on 12/26/25.
//

#ifndef ATMOSIL_ROOM_H
#define ATMOSIL_ROOM_H

#include <set>
#include <vector>

#include <SFML/Graphics.hpp>

namespace atmosil::atmo {

    class Space {
    public:

        Space() = delete;
        Space(const int &rows, const int &cols, const float &cell_size);

        void UpdateAtmosphereStep();
        void UpdateAtmosphereStepWithDt(float delta);

        void Advect(float dt);

        const std::vector<float> &GetGasPressure() { return pressure_; };\

        void AddWall(const int x, const int y);

        [[nodiscard]] const sf::Vector2f &GetWindVelocityAt(float x, float y) const;
        [[nodiscard]] const float &GetPressureAt(float x, float y) const;



    private:
        std::vector<float> pressure_{};
        std::vector<sf::Vector2f> wind_{};
        std::set<int> walls_;

        int rows_;
        int cols_;
        float cell_size_;


    };

} // namespace atmosil::atmo


#endif // ATMOSIL_ROOM_H
