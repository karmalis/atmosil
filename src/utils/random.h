//
// Created by voltairepunk on 9/21/25.
//

#ifndef FLYWHEEL_RANDOM_H
#define FLYWHEEL_RANDOM_H
#include <cstddef>
#include <random>
#include <algorithm>

namespace util::algo {
    template <typename T>
    T generate(std::size_t _);

    template <typename T>
    T generate(T min, T max);

    static const std::vector possible_random_chars {
        '0', '1', '2', '3', '4',
        '5', '6', '7', '8', '9',
        'A', 'B', 'C', 'D', 'E', 'F',
        'G', 'H', 'I', 'J', 'K',
        'L', 'M', 'N', 'O', 'P',
        'Q', 'R', 'S', 'T', 'U',
        'V', 'W', 'X', 'Y', 'Z',
        'a', 'b', 'c', 'd', 'e', 'f',
        'g', 'h', 'i', 'j', 'k',
        'l', 'm', 'n', 'o', 'p',
        'q', 'r', 's', 't', 'u',
        'v', 'w', 'x', 'y', 'z'
    };

    template<typename T>
    T generate(std::size_t _) {
        throw std::exception();
    }

    template<>
    inline std::string generate<std::string>(const std::size_t length) {
        std::string str(length, 0);
        std::default_random_engine rng(std::random_device{}());
        std::mt19937 mt(rng());
        std::uniform_int_distribution<unsigned long> dist(0, possible_random_chars.size() - 1);


        std::generate_n(str.begin(), length, [&dist, &mt]{
            return possible_random_chars[dist(mt)];
        });

        return str;
    }

    template<typename T>
    T generate(const T min, const T max) {
        std::default_random_engine rd(std::random_device{}());
        std::mt19937 mt(rd());

        std::uniform_int_distribution<T> dist{min, max};
        return dist(mt);
    }

    template<>
    inline float generate(const float min, const float max) {
        std::default_random_engine rd(std::random_device{}());
        std::mt19937 mt(rd());

        std::uniform_real_distribution dist{min, max};
        return dist(mt);
    }

    template<>
    inline double generate(const double min, const double max) {
        const auto v = static_cast<double>(generate<float>(0., 1.));
        return min + v * (max - min);
    }
}

#endif //FLYWHEEL_RANDOM_H