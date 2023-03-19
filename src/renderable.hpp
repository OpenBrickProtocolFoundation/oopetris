#pragma once

#include "application.hpp"

struct Renderable {
    virtual void render(const Application& app) const = 0;
};
