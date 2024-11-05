


#include "ai.hpp"


oopetris::AI::AI(std::string name) : m_name{ name } { }

oopetris::AI::~AI() = default;

std::string oopetris::AI::name() const {
    return m_name;
}
