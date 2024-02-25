#pragma once

#include "graphics/renderer.hpp"
#include "graphics/text.hpp"
#include "helper/timer.hpp"
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
        Texture m_text_texture;
        shapes::Rect m_viewport;
        shapes::Rect m_cursor_rect;
        u32 scaled_text_size;
        bool cursor_shown{ true };
        helper::Timer timer;

        explicit TextInput(
                ServiceProvider* service_provider,
                Font font,
                const Color& color,
                usize focus_id,
                const shapes::Rect& fill_rect,
                Layout layout
        )
            : Widget{ layout },
              Focusable{ focus_id },
              Hoverable{ fill_rect },
              m_service_provider{ service_provider },
              m_font{ std::move(font) },
              m_color{ color },
              m_text_rect{ layout.get_rect() },
              m_text_texture{ service_provider->renderer().get_texture_for_render_target(
                      shapes::Point(1, 1) // this is a dummy point!
              ) },
              scaled_text_size{ 0 },
              timer{ [this]() { this->cursor_shown = !this->cursor_shown; }, std::chrono::milliseconds(500) } {
            recalculate_textures(false);
        }

    public:
        explicit TextInput(
                ServiceProvider* service_provider,
                Font font,
                const Color& color,
                usize focus_id,
                std::pair<double, double> size,
                Alignment alignment,
                Layout layout
        )
            : TextInput{ service_provider,
                         std::move(font),
                         color,
                         focus_id,
                         ui::get_rectangle_aligned(
                                 layout,
                                 { static_cast<u32>(size.first * layout.get_rect().width()),
                                   static_cast<u32>(size.second * layout.get_rect().height()) },
                                 alignment
                         ),
                         layout } { }

        ~TextInput() override {
            on_unfocus();
        }


        void update() override {
            // update the timer every frame (in which we have the focus), to check if the time has passed, to have a blinking cursor
            if (has_focus()) {
                timer.check();
            }
        }

        //TODO: how to handle text limits (since texture for texts on the gpu can't get unlimitedly big, maybe use software texture?)
        void render(const ServiceProvider& service_provider) const override {
            const auto background_color = has_focus()    ? Color(0x6D, 0x6E, 0x6D)
                                          : is_hovered() ? Color(0x47, 0x47, 0x47)
                                                         : Color(0x3A, 0x3B, 0x39);

            const auto& renderer = service_provider.renderer();
            const auto& layout_rect = layout().get_rect();

            renderer.draw_rect_filled(layout_rect, background_color);
            if (not m_text.empty()) {

                auto to_rect = layout_rect;

                // the text fits, so we don't have to scroll, the to_rect isn't the whole layout_rect
                if (scaled_text_size < static_cast<u32>(layout_rect.width())) {
                    to_rect = shapes::Rect{ layout_rect.top_left.x, layout_rect.top_left.y,
                                            static_cast<int>(scaled_text_size), layout_rect.height() };
                }


                renderer.draw_texture(m_text_texture, m_viewport, to_rect);
            }

            if (cursor_shown and has_focus()) {
                renderer.draw_rect_filled(m_cursor_rect, m_color);
            }
        }

        helper::BoolWrapper<ui::EventHandleType>
        handle_event(const SDL_Event& event, const Window* window) // NOLINT(readability-function-cognitive-complexity)
                override {
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
                                    recalculate_textures(true);
                                    return true;
                                }

                                remove_at_cursor();
                                recalculate_textures(true);
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

                            recalculate_textures(false);
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

                            recalculate_textures(false);
                            return true;
                        }
                        case SDLK_v: {
                            if ((event.key.keysym.mod & KMOD_CTRL) != 0) {

                                if (SDL_HasClipboardText() != SDL_FALSE) {
                                    char* text = SDL_GetClipboardText();
                                    if (SDL_strlen(static_cast<const char*>(event.text.text)) == 0) {
                                        return true;
                                    }
                                    const std::string new_string{ text };
                                    SDL_free( // NOLINT(cppcoreguidelines-no-malloc, cppcoreguidelines-owning-memory)
                                            text
                                    );
                                    add_string(new_string);
                                    recalculate_textures(true);
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
                    if (SDL_strlen(static_cast<const char*>(event.text.text)) == 0 or event.text.text[0] == '\n') {
                        return true;
                    }

                    const std::string new_string{ static_cast<const char*>(event.text.text) };
                    add_string(new_string);
                    recalculate_textures(true);

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
        void recalculate_textures(bool text_changed) {

            const auto layout_rect = layout().get_rect();
            const auto& renderer = m_service_provider->renderer();

            constexpr auto cursor_width = 4;
            const auto unmoved_cursor = shapes::Rect(
                    0, static_cast<int>(static_cast<double>(layout_rect.height()) * 0.05), cursor_width,
                    static_cast<int>(static_cast<double>(layout_rect.height()) * 0.90)
            );

            if (m_text.empty()) {
                m_viewport = shapes::Rect{ 0, 0, 0, 0 };

                m_cursor_rect = unmoved_cursor.move(layout_rect.top_left);


                if (text_changed) {
                    m_text_texture =
                            renderer.get_texture_for_render_target(shapes::Point(1, 1) // this is a dummy point!
                            );
                    scaled_text_size = 0;
                }

                return;
            }


            double ratio = static_cast<double>(m_text_texture.size().y) / static_cast<double>(layout_rect.height());

            if (text_changed) {
                m_text_texture = renderer.prerender_text(m_text, m_font, m_color);

                ratio = static_cast<double>(m_text_texture.size().y) / static_cast<double>(layout_rect.height());

                scaled_text_size = static_cast<u32>(static_cast<double>(m_text_texture.size().x) / ratio);
            }

            m_viewport = shapes::Rect{ 0, 0, m_text_texture.size().x, m_text_texture.size().y };

            int cursor_offset = 0;

            if (cursor_position != 0) {
                // calculate substring that is before the cursor

                std::string sub_string{};

                for (auto [current_iterator, i] = std::tuple{ m_text.begin(), u32{ 0 } };; ++i) {

                    if (i == cursor_position) {
                        break;
                    }

                    if (current_iterator == m_text.end()) {
                        break;
                    }

                    utf8::append(utf8::next(current_iterator, m_text.end()), sub_string);
                }

                int w = 0;
                int h = 0;
                const int result = TTF_SizeUTF8(m_font.get(), sub_string.c_str(), &w, &h);

                if (result < 0) {
                    throw std::runtime_error("Error during SDL_TTF_SizeUTF8: " + std::string{ SDL_GetError() });
                }

                const double ratio_sub_string = static_cast<double>(h) / static_cast<double>(layout_rect.height());

                cursor_offset = static_cast<int>(static_cast<double>(w) / ratio_sub_string);
            }

            m_cursor_rect = unmoved_cursor.move(layout_rect.top_left).move(shapes::Point{ cursor_offset, 0 });

            // the text doesn't fit, so we have to scroll,we have to offset the viewport and the cursor_rect accordingly and center the viewport around the cursor
            if (scaled_text_size >= static_cast<u32>(layout_rect.width())) {

                const auto cursor_middle = cursor_offset - (layout_rect.width() / 2);

                auto final_offset = cursor_middle;

                if (cursor_middle < 0) {
                    final_offset = 0;
                } else if ((scaled_text_size - cursor_middle) < static_cast<u32>(layout_rect.width())) {
                    final_offset = scaled_text_size - static_cast<u32>(layout_rect.width());

                    // if we reached the end, we have shift the cursor for some  pixels, so that we can see the bar xD, thsi strectehces it slightly, but it's a simple solution
                    if ((scaled_text_size - cursor_middle) <= static_cast<u32>(layout_rect.width() / 2)) {
                        final_offset += 2 * cursor_width;
                    }
                }

                m_viewport = shapes::Rect{ static_cast<int>(static_cast<double>(final_offset) * ratio), 0,
                                           static_cast<int>(static_cast<double>(layout_rect.width()) * ratio),
                                           m_text_texture.size().y };
                m_cursor_rect = m_cursor_rect.move(shapes::Point{ -final_offset, 0 });
            }
        }

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

        void on_focus() override {
            m_service_provider->event_dispatcher().start_text_input(layout().get_rect());
            timer.start();
        }

        void on_unfocus() override {
            m_service_provider->event_dispatcher().stop_text_input();
            timer.stop();
        }
    };
} // namespace ui
