#pragma once

#include "event_listener.hpp"
#include "scene.hpp"
#include "ui/label.hpp"

struct MainMenuScene : public Scene {
private:
    ui::Label m_heading;
    bool m_should_end{ false };
    bool m_should_exit{ false };

public:
    explicit MainMenuScene(TetrisApplication* application);

    [[nodiscard]] UpdateResult update() override;
    void render(const Application& app) override;
    [[nodiscard]] bool handle_event(const SDL_Event& event) override;
};
