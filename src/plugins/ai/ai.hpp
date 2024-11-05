
#pragma once

#include <string>

#include "input/game_input.hpp"

namespace oopetris {


    struct AI {
    private:
        std::string m_name;


    public:
        explicit AI(std::string name);

        virtual ~AI();

        [[nodiscard]] std::string name() const;

        virtual std::shared_ptr<input::GameInput> input() = 0;
    };


} // namespace oopetris
