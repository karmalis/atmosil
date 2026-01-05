//
// Created by voltairepunk on 12/26/25.
//

#include "Space.h"

#include "../utils/random.h"

#include <iostream>

namespace atmosil::atmo {

    constexpr const float kFlowConst = 0.1;
    constexpr const sf::Vector2f kZeroPos = sf::Vector2f(0.f, 0.f);
    constexpr const float kZeroPressure = 0.f;

    struct PotentialNeighbor {
        int idx;
        bool valid;
    };

    Space::Space(const int &rows, const int &cols, const float &cell_size) :
        rows_(rows), cols_(cols), cell_size_(cell_size) {
        pressure_.reserve(rows_ * cols_);

        for (auto idx = 0; idx < rows_ * cols_; idx++) {
            const auto val = util::algo::generate<float>(0.0, 0.7);
            pressure_.push_back(0.f);

            // if (((idx) % cols) == 0) {
            //     pressure_.push_back(1.0);
            // } else {
            //     pressure_.push_back(0);
            // }
        }

        // pressure_[0] = 1.0f;
        pressure_[cols_ + 1] = 1.0f;
        pressure_[cols_ + 2] = 1.0f;
        pressure_[cols_ * 2 + 1] = 1.0f;
        pressure_[cols_ * 2 + 2] = 1.0f;
        pressure_[cols_ * 2 + 5] = 1.0f;
        // pressure_[cols_ * 2] = 1.0f;

        wind_.reserve(rows_ * cols_);
        for (auto idx = 0; idx < rows_ * cols_; idx++) {
            wind_.emplace_back(0.f, 0.f);
        }
    }

    void Space::UpdateAtmosphereStep() { UpdateAtmosphereStepWithDt(1.f); }
    void Space::UpdateAtmosphereStepWithDt(const float delta) {
        auto new_pressure = pressure_;
        std::ranges::fill(wind_, sf::Vector2f(0.f, 0.f));

        for (int y = 0; y < rows_; y++) {
            for (int x = 0; x < cols_; x++) {
                const auto idx = y * cols_ + x;
                if (pressure_[idx] <= 0)
                    continue;

                // TODO: Consider 8 surrounding spaces for gas expansion
                const PotentialNeighbor potential_neighbors[] = {
                        {idx + 1, x < cols_ - 1}, // right
                        {idx - 1, x > 0}, // left
                        {idx + cols_, y < rows_ - 1}, // bottom
                        {idx - cols_, y > 0}, // top
                };

                std::vector flow{0.f, 0.f, 0.f, 0.f};
                float total_outflow = 0.f;

                int n_it = 0;
                for (const auto neighbor: potential_neighbors) {

                    if (!neighbor.valid || walls_.contains(neighbor.idx)) {
                        n_it++;
                        continue;
                    }

                    if (const auto diff = pressure_[idx] - pressure_[neighbor.idx]; diff > 0) {
                        const auto amount_to_move = diff * kFlowConst * delta;

                        sf::Vector2f flow_dir(0.f, 0.f);

                        if (n_it == 0) flow_dir = {1.f, 0.f};
                        else if (n_it == 1) flow_dir = {-1.f, 0.f};
                        else if (n_it == 2) flow_dir = {0.f, 1.f};
                        else if (n_it == 3) flow_dir = {0.f, -1.f};

                        const sf::Vector2f flow_vec = flow_dir * amount_to_move;

                        wind_[idx] += flow_vec;
                        wind_[neighbor.idx] += flow_vec;

                        flow[n_it] = amount_to_move;
                        total_outflow += amount_to_move;
                        new_pressure[idx] -= amount_to_move;
                        new_pressure[neighbor.idx] += amount_to_move;
                        n_it++;
                    }
                }

                // const auto vel_x = flow[0] - flow[1];
                // const auto vel_y = flow[2] - flow[3];
                //
                // wind_[idx].x = vel_x;
                // wind_[idx].y = vel_y;
            }
        }
        pressure_ = new_pressure;
    }

    void Space::AddWall(const int x, const int y) {
        const auto idx = y * cols_ + x;
        walls_.insert(idx);
    }

    const sf::Vector2f &Space::GetWindVelocityAt(const float x, const float y) const {
        if (x < 0 || x >= static_cast<float>(cols_ * cell_size_) ||
        y < 0 || y >= static_cast<float>(rows_ * cell_size_)) {
            return kZeroPos;
        }

        const auto _x = static_cast<int>(x / cell_size_);
        const auto _y = static_cast<int>(y / cell_size_);

        if (_x >= cols_ || _y >= rows_) return kZeroPos;

        const auto idx = _y * cols_ + _x;
        return wind_[idx];
    }
    const float &Space::GetPressureAt(const float x, const float y) const {
        if (x < 0 || x >= static_cast<float>(cols_ * cell_size_) ||
        y < 0 || y >= static_cast<float>(rows_ * cell_size_)) {
            return kZeroPressure;
        }

        const auto _x = static_cast<int>(x / cell_size_);
        const auto _y = static_cast<int>(y / cell_size_);

        if (_x >= cols_ || _y >= rows_) return kZeroPressure;

        const auto idx = _y * cols_ + _x;
        if (idx > wind_.size() || idx < 0) {
            return kZeroPressure;
        }

        return pressure_[idx];
    }





} // namespace atmosil::atmo
