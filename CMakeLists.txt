cmake_minimum_required(VERSION 3.25)
project(oopetris)

set(CMAKE_CXX_STANDARD 23)

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)
find_package(tl-optional CONFIG REQUIRED)

set(TARGET_LIST oopetris SDL2::SDL2-static)

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
        src/tetris_application.hpp
        src/bag.cpp
        src/bag.hpp
        src/font.cpp
        src/font.hpp
        src/text.cpp
        src/text.hpp
        src/grid.cpp
        src/input.hpp
        src/event_dispatcher.cpp
        src/event_dispatcher.hpp
        src/event_listener.hpp
        src/input.cpp
        src/settings.hpp
        src/key_codes.hpp
        src/magic_enum_wrapper.hpp
        src/types.hpp
        src/random.cpp
        src/random.hpp
        src/recording.hpp
        src/input_event.hpp
        src/utils.hpp
        src/utils.cpp
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

target_link_libraries(oopetris PRIVATE spdlog::spdlog spdlog::spdlog_header_only)
target_link_libraries(oopetris PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(oopetris PRIVATE magic_enum::magic_enum)
target_link_libraries(oopetris PRIVATE $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>)
target_link_libraries(oopetris PRIVATE tl::optional)

# static runtime library
set_property(TARGET ${target} PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
