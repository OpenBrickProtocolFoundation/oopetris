

#include "game/simulation.hpp"
#include "utils/helper.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>


TEST(Simulation, InvalidFilePath) {

    std::filesystem::path path = "__INVALID_PATH";

    auto maybe_simulation = Simulation::get_replay_simulation(path);

    ASSERT_THAT(maybe_simulation, ExpectedHasError())
            << "Path was: " << path << "\nError: " << maybe_simulation.error();
    ASSERT_THAT(
            maybe_simulation.error(),
            ("an error occurred while reading recording: unable to load recording from file \"" + path.string() + "\"")
    );
}

TEST(Simulation, ValidRecordingsFile) {

    std::filesystem::path path = "./test_rec_valid.rec";

    auto maybe_simulation = Simulation::get_replay_simulation(path);

    ASSERT_THAT(maybe_simulation, ExpectedHasValue())
            << "Path was: " << path << "\nError: " << maybe_simulation.error();
}
