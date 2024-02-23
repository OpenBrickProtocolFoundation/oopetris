#pragma once

#include "graphics/text.hpp"
#include "input/event_dispatcher.hpp"
#include "ui/focusable.hpp"
#include "ui/hoverable.hpp"
#include "ui/widget.hpp"

#include <string>
#include <utf8.h>

namespace ui {
    struct TextInput final : public Widget, public Focusable, public Hoverable {
    private:
        std::string m_text{};
        u32 cursor_position{ 0 };
        ServiceProvider* m_service_provider;
        Font m_font;
        Color m_color;
        shapes::Rect m_text_rect;
        //  Texture text;
        //  shapes::Rect m_viewport;
        bool cursor_shown{ false };


        explicit TextInput(
                ServiceProvider* service_provider,
                const Font& font,
                const Color& color,
                usize focus_id,
                const shapes::Rect& fill_rect,
                Layout layout
        )
            : Widget{ layout },
              Focusable{ focus_id },
              Hoverable{ fill_rect },
              m_service_provider{ service_provider },
              m_font{ font },
              m_color{ color },
              m_text_rect{ layout.get_rect() } { }

    public:
        explicit TextInput(
                ServiceProvider* service_provider,
                const Font& font,
                const Color& color,
                usize focus_id,
                std::pair<double, double> size,
                Alignment alignment,
                Layout layout
        )
            : TextInput{ service_provider,
                         font,
                         color,
                         focus_id,
                         ui::get_rectangle_aligned(
                                 layout,
                                 { static_cast<u32>(size.first * layout.get_rect().width()),
                                   static_cast<u32>(size.second * layout.get_rect().height()) },
                                 alignment
                         ),
                         layout } { }

        ~TextInput() {
            on_unfocus();
        }


        void render(const ServiceProvider& service_provider) const override {
            const auto background_color = has_focus()    ? Color(0x6D, 0x6E, 0x6D)
                                          : is_hovered() ? Color(0x47, 0x47, 0x47)
                                                         : Color(0x3A, 0x3B, 0x39);

            service_provider.renderer().draw_rect_filled(layout().get_rect(), background_color);
            if (not m_text.empty()) {
                const Text text{ &service_provider, m_text, m_font, m_color, m_text_rect };
                text.render(service_provider);
            }
        }

        helper::BoolWrapper<ui::EventHandleType> handle_event(const SDL_Event& event, const Window* window) override {
            if (const auto hover_result = detect_hover(event, window); hover_result) {
                if (hover_result.is(ActionType::Clicked)) {
                    return { true, EventHandleType::RequestFocus };
                    return true;
                }

                return true;
            }

            switch (event.type) {
                case SDL_KEYDOWN: {
                    switch (event.key.keysym.sym) {
                        case SDLK_RETURN: {
                            on_unfocus();
                            return { true, EventHandleType::RequestUnFocus };
                        }
                        case SDLK_BACKSPACE: {
                            const auto remove_all = (event.key.keysym.mod & KMOD_CTRL) != 0;

                            if (not m_text.empty()) {
                                if (remove_all) {
                                    m_text = "";
                                    cursor_position = 0;
                                    return true;
                                }

                                remove_at_cursor();
                            }

                            return true;
                        }
                        case SDLK_LEFT: {
                            if (cursor_position != 0) {
                                if ((event.key.keysym.mod & KMOD_CTRL) != 0) {
                                    cursor_position = 0;
                                } else {
                                    --cursor_position;
                                }
                            }

                            return true;
                        }
                        case SDLK_RIGHT: {
                            const auto current_string_length = utf8::distance(m_text.cbegin(), m_text.cend());
                            if (cursor_position < current_string_length) {
                                if ((event.key.keysym.mod & KMOD_CTRL) != 0) {
                                    cursor_position = static_cast<u32>(current_string_length);

                                } else {
                                    ++cursor_position;
                                }
                            }

                            return true;
                        }
                        case SDLK_v: {
                            if ((event.key.keysym.mod & KMOD_CTRL) != 0) {

                                if (SDL_HasClipboardText() != SDL_FALSE) {
                                    char* text = SDL_GetClipboardText();
                                    if (SDL_strlen(event.text.text) == 0) {
                                        return true;
                                    }
                                    std::string new_string{ text };
                                    SDL_free(text);
                                    add_string(new_string);
                                }

                                return true;
                            }
                            return false;
                        }
                        default:
                            break;
                    }
                    break;
                }
                case SDL_TEXTINPUT: {
                    if (SDL_strlen(event.text.text) == 0 || event.text.text[0] == '\n') {
                        return true;
                    }

                    std::string new_string{ event.text.text };
                    add_string(new_string);

                    return true;
                }
                case SDL_TEXTEDITING:
                    return true;
                default:
                    break;
            }


            return false;
        }

    private:
        bool add_string(const std::string& add) {

            const auto is_valid = utf8::is_valid(add.cbegin(), add.cend());

            if (not is_valid) {
                return false;
            }

            const auto current_string_length = utf8::distance(m_text.cbegin(), m_text.cend());

            // cursor_position is the range [0, length] (inclusively !)
            if (cursor_position > current_string_length) {
                throw std::runtime_error("cursor_postion is invalid!");
            }

            std::string result{};

            for (auto [current_iterator, i] = std::tuple{ m_text.begin(), u32{ 0 } };; ++i) {

                if (i == cursor_position) {
                    for (auto add_iterator = add.begin(); add_iterator != add.end();) {
                        utf8::append(utf8::next(add_iterator, add.end()), result);
                    }
                }

                if (current_iterator == m_text.end()) {
                    break;
                }

                utf8::append(utf8::next(current_iterator, m_text.end()), result);
            }

            m_text = result;

            const auto add_string_length = utf8::distance(add.cbegin(), add.cend());
            cursor_position += static_cast<u32>(add_string_length);

            return true;
        }

        bool remove_at_cursor() {

            if (cursor_position == 0) {
                return false;
            }

            const auto current_string_length = utf8::distance(m_text.cbegin(), m_text.cend());

            // cursor_position is the range [0, length] (inclusively !)
            if (cursor_position > current_string_length) {
                throw std::runtime_error("cursor_postion is invalid!");
            }

            auto start = m_text.begin();
            utf8::advance(start, cursor_position - 1, m_text.end());
            auto end = start;
            utf8::next(end, m_text.end());
            m_text.erase(start, end);

            --cursor_position;
            return true;
        }

        void recalculate_cursor() {

            const auto current_string_length = utf8::distance(m_text.cbegin(), m_text.cend());

            // cursor_position is the range [0, length] (inclusively !)
            if (cursor_position > current_string_length) {
                throw std::runtime_error("cursor_postion is invalid!");
            }
        }

        void on_focus() override {
            m_service_provider->event_dispatcher().start_text_input(layout().get_rect());
        }

        void on_unfocus() override {
            m_service_provider->event_dispatcher().stop_text_input();
        }
    };
} // namespace ui
