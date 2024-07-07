

#include "game/simulation.hpp"
#include "utils/helper.hpp"
#include "./helper/service_provider.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>


TEST(Simulation, InvalidFilePath) {

    auto service_provider = std::make_shared<DummyServiceProvider>();

    std::filesystem::path path = "__INVALID_PATH";

    auto maybe_simulation = Simulation::get_replay_simulation(service_provider.get(), path);

    ASSERT_THAT(maybe_simulation, ExpectedHasError()) << "Path was: " << path << "Error: " << maybe_simulation.error();
}
