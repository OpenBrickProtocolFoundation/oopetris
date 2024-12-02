#include <core/game/tetromino_type.hpp>
#include <core/helper/point.hpp>

#include "game/graphic_helpers.hpp"
#include "graphics/rect.hpp"
#include "graphics/renderer.hpp"
#include "graphics/window.hpp"
#include "helper/platform.hpp"
#include "loading_screen.hpp"
#include "manager/service_provider.hpp"
#include "scenes/logo/logo.hpp"
#include "ui/layout.hpp"

#include <numbers>


scenes::LoadingScreen::LoadingScreen(ServiceProvider* service_provider)
    : m_logo{ logo::get_logo(service_provider) },
      m_spinner{ ui::FullScreenLayout{ service_provider->window() }, true } {

    const auto& window = service_provider->window();

    const auto layout = window.size();

    constexpr const auto logo_width_percentage = 0.8;

    constexpr const auto start_x = (1.0 - logo_width_percentage) / 2.0;

    const auto window_ratio = static_cast<double>(layout.x) / static_cast<double>(layout.y);

    const auto logo_ratio = static_cast<double>(logo::height) / static_cast<double>(logo::width) * window_ratio;

    const auto logo_height_percentage = logo_width_percentage * logo_ratio;

    m_logo_rect = ui::RelativeLayout(window, start_x, 0.05, logo_width_percentage, logo_height_percentage).get_rect();
}


void scenes::LoadingScreen::update() {
    m_spinner.update();
}

void scenes::LoadingScreen::render(const ServiceProvider& service_provider) const {
    // NOTE: this already fills the background
    m_spinner.render(service_provider);

    service_provider.renderer().draw_texture(m_logo, m_logo_rect);
}
