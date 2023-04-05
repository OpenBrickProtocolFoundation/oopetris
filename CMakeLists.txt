cmake_minimum_required(VERSION 3.25)
project(oopetris)

set(CMAKE_CXX_STANDARD 23)

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(SDL2_net CONFIG REQUIRED)

add_executable(oopetris
        src/application.cpp
        src/application.hpp
        src/bag.cpp
        src/bag.hpp
        src/color.hpp
        src/event_dispatcher.cpp
        src/event_dispatcher.hpp
        src/event_listener.hpp
        src/font.cpp
        src/font.hpp
        src/game_manager.cpp
        src/game_manager.hpp
        src/grid.cpp
        src/grid.hpp
        src/input.cpp
        src/input.hpp
        src/local_multiplayer.cpp
        src/main.cpp
        src/mino.cpp
        src/mino.hpp
        src/network/connection_manager.cpp
        src/network/network_manager.cpp
        src/network/network_transportable.cpp
        src/play_manager.cpp
        src/point.hpp
        src/rect.hpp
        src/renderer.cpp
        src/renderer.hpp
        src/sdl_context.cpp
        src/sdl_context.hpp
        src/tetris_application.cpp
        src/tetris_application.hpp
        src/tetromino_type.cpp
        src/tetromino_type.hpp
        src/tetromino.hpp
        src/text.cpp
        src/text.hpp
        src/window.cpp
        src/window.hpp
)

if(MSVC)
        target_compile_options(oopetris PRIVATE /W4 /WX)
else()
        target_compile_options(oopetris PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

target_link_libraries(oopetris
        PRIVATE
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)

target_link_libraries(oopetris PRIVATE $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>)

target_link_libraries(oopetris PRIVATE $<IF:$<TARGET_EXISTS:SDL2_net::SDL2_net>,SDL2_net::SDL2_net,SDL2_net::SDL2_net-static>)
