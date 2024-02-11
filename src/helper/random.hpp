#pragma once

#include "helper/utils.hpp"
#include <random>

struct Random {
public:
    using Seed = std::mt19937_64::result_type;

private:
    std::mt19937_64 m_generator;
    Seed m_seed{};
    std::uniform_real_distribution<double> m_uniform_real_distribution;

public:
    Random();
    explicit Random(std::mt19937_64::result_type seed);

    template<utils::integral Integer>
    [[nodiscard]] Integer random(const Integer upper_bound_exclusive) {
        auto distribution = std::uniform_int_distribution<Integer>{ 0, upper_bound_exclusive - 1 };
        return distribution(m_generator);
    }

    [[nodiscard]] double random();
    [[nodiscard]] Seed seed() const;
    void seed(Seed seed);
    static Seed generate_seed();
};
