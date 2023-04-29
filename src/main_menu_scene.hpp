#pragma once

#include "event_listener.hpp"
#include "scene.hpp"
#include "ui/label.hpp"

struct MainMenuScene : public Scene, public EventListener {
private:
    ui::Label m_heading;
    bool m_should_end{ false };

public:
    explicit MainMenuScene(TetrisApplication* application);
    ~MainMenuScene();

    [[nodiscard]] UpdateResult update() override;
    void render(const Application& app) override;
    void handle_event(const SDL_Event& event) override;
};
