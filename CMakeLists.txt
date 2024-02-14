cmake_minimum_required(VERSION 3.25)
project(oopetris)

set(CMAKE_CXX_STANDARD 23)

find_package(argparse CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)
find_package(tl-optional CONFIG REQUIRED)
find_package(tl-expected CONFIG REQUIRED)

set(TARGET_LIST oopetris SDL2::SDL2-static)

add_executable(oopetris
        src/application.cpp
        src/application.hpp
        src/game/bag.cpp
        src/game/bag.hpp
        src/game/grid.cpp
        src/game/grid.hpp
        src/game/mino_stack.cpp
        src/game/mino_stack.hpp
        src/game/mino.cpp
        src/game/mino.hpp
        src/game/tetrion_snapshot.cpp
        src/game/tetrion_snapshot.hpp
        src/game/tetrion.cpp
        src/game/tetrion.hpp
        src/game/tetromino_type.hpp
        src/game/tetromino.hpp
        src/graphics/point.hpp
        src/graphics/rect.hpp
        src/graphics/renderer.cpp
        src/graphics/renderer.hpp
        src/graphics/sdl_context.cpp
        src/graphics/sdl_context.hpp
        src/graphics/text.cpp
        src/graphics/text.hpp
        src/graphics/texture.hpp
        src/graphics/window.cpp
        src/graphics/window.hpp
        src/helper/clock_source.cpp
        src/helper/clock_source.hpp
        src/helper/color.hpp
        src/helper/command_line_arguments.hpp
        src/helper/constants.hpp
        src/helper/expected.hpp
        src/helper/git_helper.hpp
        src/helper/magic_enum_wrapper.hpp
        src/helper/optional.hpp
        src/helper/random.cpp
        src/helper/random.hpp
        src/helper/static_string.hpp
        src/helper/types.hpp
        src/helper/utils.cpp
        src/helper/utils.hpp
        src/input/controls.hpp
        src/input/event_dispatcher.cpp
        src/input/event_dispatcher.hpp
        src/input/event_listener.hpp
        src/input/input_event.hpp
        src/input/input.cpp
        src/input/input.hpp
        src/input/key_codes.hpp
        src/main.cpp
        src/manager/font.cpp
        src/manager/font.hpp
        src/manager/music_manager.cpp
        src/manager/music_manager.hpp
        src/manager/recording.hpp
        src/manager/resource_manager.hpp
        src/manager/service_provider.hpp
        src/manager/settings.hpp
        src/platform/capabilities.cpp
        src/platform/capabilities.hpp
        src/platform/switch_buttons.hpp
        src/scenes/about_page/about_page.cpp
        src/scenes/about_page/about_page.hpp
        src/scenes/game_over/game_over.cpp
        src/scenes/game_over/game_over.hpp
        src/scenes/ingame/ingame.cpp
        src/scenes/ingame/ingame.hpp
        src/scenes/main_menu/main_menu.cpp
        src/scenes/main_menu/main_menu.hpp
        src/scenes/pause/pause.cpp
        src/scenes/pause/pause.hpp
        src/scenes/scene_id.hpp
        src/scenes/scene.cpp
        src/scenes/scene.hpp
        src/scenes/settings_menu/settings_menu.cpp
        src/scenes/settings_menu/settings_menu.hpp
        src/ui/button.hpp
        src/ui/focusable.hpp
        src/ui/grid_layout.hpp
        src/ui/hoverable.hpp
        src/ui/image_view.hpp
        src/ui/label.hpp
        src/ui/layout.cpp
        src/ui/layout.hpp
        src/ui/link_label.hpp
        src/ui/slider.hpp
        src/ui/tile_layout.hpp
        src/ui/widget.hpp
        )

foreach (target ${TARGET_LIST})
    # set warning levels
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        message("MSVC build")
        if (CMAKE_BUILD_TYPE STREQUAL "Release")
            target_compile_options(${target} INTERFACE /W4 /WX /permissive-)
        else ()
            target_compile_options(${target} INTERFACE /W4 /WX /permissive-)
        endif ()
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        message("GCC build")
        if (CMAKE_BUILD_TYPE STREQUAL "Release")
            target_compile_options(${target} INTERFACE -Wall -Wextra -Werror -pedantic -Wconversion)
        else ()
            target_compile_options(${target} INTERFACE -Wall -Wextra -Werror -pedantic -Wconversion)
        endif ()
    endif ()

    # define DEBUG_BUILD
    target_compile_definitions(${target} INTERFACE "$<$<CONFIG:DEBUG>:DEBUG_BUILD>")


    # static runtime library
    set_property(TARGET ${target} PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endforeach ()

target_link_libraries(oopetris
        PRIVATE
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        )

target_link_libraries(oopetris PRIVATE argparse::argparse)
target_link_libraries(oopetris PRIVATE spdlog::spdlog spdlog::spdlog_header_only)
target_link_libraries(oopetris PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(oopetris PRIVATE magic_enum::magic_enum)
target_link_libraries(oopetris PRIVATE $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>)
target_link_libraries(oopetris PRIVATE $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>)
target_link_libraries(oopetris PRIVATE $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>)
target_link_libraries(oopetris PRIVATE tl::optional)
target_link_libraries(oopetris PRIVATE tl::expected)

target_compile_definitions(oopetris PUBLIC AUDIO_WITH_FLAC_SUPPORT AUDIO_WITH_MP3_SUPPORT AUDIO_PREFER_FLAC)

target_include_directories(oopetris PRIVATE "src/")

# static runtime library
set_property(TARGET ${target} PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
