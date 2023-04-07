#include "random.hpp"
#include <chrono>

Random::Random() : Random{ generate_seed() } { }

Random::Random(const Seed seed) {
    this->seed(seed);
}

double Random::random() {
    return m_uniform_real_distribution(m_generator);
}

Random::Seed Random::seed() const {
    return m_seed;
}

void Random::seed(Random::Seed seed) {
    m_generator.seed(seed);
    m_seed = seed;
}

Random::Seed Random::generate_seed() {
    return std::chrono::system_clock::now().time_since_epoch().count();
}
