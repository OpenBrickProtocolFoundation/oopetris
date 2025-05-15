#pragma once

#include "./export_symbols.hpp"
#include "./utils.hpp"

#include <random>

struct Random {
public:
    using Seed = std::mt19937_64::result_type;

private:
    std::mt19937_64 m_generator;
    Seed m_seed{};
    std::uniform_real_distribution<double> m_uniform_real_distribution;

public:
    OOPETRIS_CORE_EXPORTED Random();
    OOPETRIS_CORE_EXPORTED explicit Random(std::mt19937_64::result_type seed);

    template<std::integral Integer>
    [[nodiscard]] Integer random(const Integer upper_bound_exclusive) {
        auto distribution = std::uniform_int_distribution<Integer>{ 0, upper_bound_exclusive - 1 };
        return distribution(m_generator);
    }

    [[nodiscard]] OOPETRIS_CORE_EXPORTED double random();

    [[nodiscard]] OOPETRIS_CORE_EXPORTED Seed seed() const;

    OOPETRIS_CORE_EXPORTED void seed(Seed seed);

    OOPETRIS_CORE_EXPORTED static Seed generate_seed();
};
