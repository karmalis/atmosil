//
// Created by voltairepunk on 12/26/25.
//

#include "Space.h"

#include "../utils/random.h"

#include <iostream>

namespace atmosil::atmo {

    constexpr const int kSolverIterationCount = 20;
    constexpr const float kFlowConst = 0.1;
    constexpr const sf::Vector2f kZeroPos = sf::Vector2f(0.f, 0.f);
    constexpr const float kZeroPressure = 0.f;
    constexpr const float kDragFactor = 0.99f;

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

    void Space::UpdateAtmosphereStep() {
        UpdateAtmosphereStepWithDt(1.f);

    }
    void Space::ComputePressureAndWind(const float delta) {
        auto new_pressure = pressure_;

        for (int y = 0; y < rows_; y++) {
            for (int x = 0; x < cols_; x++) {
                const auto idx = y * cols_ + x;

                if (IsWall(idx)) continue;

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
                for (const auto& neighbor: potential_neighbors) {

                    if (!neighbor.valid || IsWall(neighbor.idx)) {
                        n_it++;
                        continue;
                    }

                    if (const auto diff = pressure_[idx] - pressure_[neighbor.idx]; diff > 0) {
                        const auto amount_to_move = diff * kFlowConst * delta;

                        sf::Vector2f flow_dir(0.f, 0.f);

                        if (n_it == 0)
                            flow_dir = {1.f, 0.f};
                        else if (n_it == 1)
                            flow_dir = {-1.f, 0.f};
                        else if (n_it == 2)
                            flow_dir = {0.f, 1.f};
                        else if (n_it == 3)
                            flow_dir = {0.f, -1.f};

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
            }
        }
        pressure_ = new_pressure;
    }
    void Space::UpdateAtmosphereStepWithDt(const float delta) {
        ComputePressureAndWind(delta);
        Project();
        Advect(delta);
        Project();

        for (auto& v : wind_) {
            v *= kDragFactor;
        }
    }

    void Space::Project() {
    std::vector p(rows_ * cols_, 0.0f);
    std::vector div(rows_ * cols_, 0.0f);

    for (auto y = 1; y < rows_ - 1; y++) {
        for (auto x = 1; x < cols_ - 1; x++) {
            const auto idx = y * cols_ + x;

            div[idx] = -0.5f * (
                 wind_[idx + 1].x - wind_[idx - 1].x +
                 wind_[idx + cols_].y - wind_[idx - cols_].y
            ) / cell_size_;

            p[idx] = 0.0f;
        }
    }

    for (auto k = 0; k < kSolverIterationCount; k++) {
        for (auto y = 1; y < rows_ - 1; y++) {
            for (auto x = 1; x < cols_ - 1; x++) {
                const auto idx = y * cols_ + x;

                if (IsWall(idx)) {
                     p[idx] = 0;
                     continue;
                }

                p[idx] = (div[idx] +
                          p[idx - 1] + p[idx + 1] +
                          p[idx - cols_] + p[idx + cols_]) / 4.0f;
            }
        }
    }

    for (auto y = 1; y < rows_ - 1; y++) {
        for (auto x = 1; x < cols_ - 1; x++) {
            const auto idx = y * cols_ + x;

            if (IsWall(idx)) continue;

            wind_[idx].x -= 0.5f * (p[idx + 1] - p[idx - 1]) * cell_size_;
            wind_[idx].y -= 0.5f * (p[idx + cols_] - p[idx - cols_]) * cell_size_;
        }
    }
}

    void Space::Advect(const float dt) {
        std::vector<float> new_pressure = pressure_;
        std::vector<sf::Vector2f> new_wind = wind_;

        const auto dt0 = dt * cell_size_;

        for (int y = 1; y < rows_ - 1; y++) {
            for (int x = 1; x < cols_ - 1; x++) {
                const auto idx = y * cols_ + x;

                if (IsWall(idx)) {
                    new_pressure[idx] = 0.0f;
                    new_wind[idx] = sf::Vector2f(0.0f, 0.0f);
                    continue;
                }

                auto x_prev = x - dt0 * wind_[idx].x;
                auto y_prev = y - dt0 * wind_[idx].y;

                if (x_prev < 0.5f) x_prev = 0.5f;
                if (x_prev > cols_ - 1.5f) x_prev = cols_ - 1.5f;
                if (y_prev < 0.5f) y_prev = 0.5f;
                if (y_prev > rows_ - 1.5f) y_prev = rows_ - 1.5f;

                const auto i0 = static_cast<int>(x_prev);
                const auto i1 = i0 + 1;
                const auto j0 = static_cast<int>(y_prev);
                const auto j1 = j0 + 1;

                const auto s1 = x_prev - i0;
                const auto s0 = 1.0f - s1;
                const auto t1 = y_prev - j0;
                const auto t0 = 1.0f - t1;

                new_pressure[idx] =
                    s0 * (t0 * pressure_[j0 * cols_ + i0] + t1 * pressure_[j1 * cols_ + i0]) +
                    s1 * (t0 * pressure_[j0 * cols_ + i1] + t1 * pressure_[j1 * cols_ + i1]);

                new_wind[idx] =
                   s0 * (t0 * wind_[j0 * cols_ + i0] + t1 * wind_[j1 * cols_ + i0]) +
                   s1 * (t0 * wind_[j0 * cols_ + i1] + t1 * wind_[j1 * cols_ + i1]);
            }
        }

        pressure_ = new_pressure;
        wind_ = new_wind;
    }

    void Space::AddWall(const int x, const int y) {
        const auto idx = y * cols_ + x;
        walls_.insert(idx);
    }
    bool Space::IsWall(const int index) const {
        if (index < 0 || index >= pressure_.size()) return true;

        return walls_.contains(index);
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
    float Space::GetTotalPressure() {
        //return std::accumulate(pressure_.begin(), pressure_.end(), 0.0f);

        auto result = 0.0f;
        for (const auto p: pressure_) {
            result += p;
        }
        return result;
    }


} // namespace atmosil::atmo
