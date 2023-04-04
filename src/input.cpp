#include "input.hpp"
#include "game_manager.hpp"

void KeyboardInput::handle_event(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_LEFT:
                m_target_game_manager->handle_input_event(Event::RotateLeft);
                break;
            case SDLK_RIGHT:
                m_target_game_manager->handle_input_event(Event::RotateRight);
                break;
            case SDLK_s:
                m_target_game_manager->handle_input_event(Event::MoveDown);
                break;
            case SDLK_a:
                m_target_game_manager->handle_input_event(Event::MoveLeft);
                break;
            case SDLK_d:
                m_target_game_manager->handle_input_event(Event::MoveRight);
                break;
            case SDLK_SPACE:
            case SDLK_w:
                m_target_game_manager->handle_input_event(Event::Drop);
                break;
        }
    }
}
