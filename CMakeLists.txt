cmake_minimum_required(VERSION 3.25)
project(oopetris)

set(CMAKE_CXX_STANDARD 14)

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)

add_executable(oopetris
        src/main.cpp
        src/application.hpp
        src/application.cpp
        src/renderer.cpp
        src/renderer.hpp
        src/sdl_context.cpp
        src/sdl_context.hpp
        src/window.cpp
        src/window.hpp
        src/color.hpp
        src/rect.hpp
        src/point.hpp
        src/tetromino.hpp
        src/mino.hpp
        src/grid.hpp
        src/tetromino_type.hpp
        src/tetromino_type.cpp
        src/mino.cpp
        src/game_manager.cpp
        src/game_manager.hpp
        src/tetris_application.cpp
        src/tetris_application.hpp
        src/tetromino.cpp
        src/bag.cpp
        src/bag.hpp
        src/font.cpp
        src/font.hpp
        src/text.cpp
        src/text.hpp src/grid.cpp)

target_link_libraries(oopetris
        PRIVATE
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        )

target_link_libraries(oopetris PRIVATE $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>)
