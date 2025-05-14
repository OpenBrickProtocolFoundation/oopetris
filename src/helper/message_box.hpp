

#pragma once

#include <core/helper/types.hpp>

#include <SDL.h>
#include <string>

#include <core/helper/types.hpp>

#include "./windows.hpp"

namespace helper {

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined(__MINGW32__) \
        || defined(__MINGW64__)
    // "WinUser.h" defines "#define MessageBox  MessageBoxA"
    // which breaks this valid code, I love windows xD.
#ifdef MessageBox
#undef MessageBox
#endif
#endif


    struct MessageBox {
        enum class Type : u8 { Error, Warning, Information };

        /***
        * \brief This blocks the current thread and should be called on the thread, the parent windows was created (if that is nullptr, it may be on another thread, but not otherwise)
        */
        OOPETRIS_GRAPHICS_EXPORTED static void
        show_simple(Type type, const std::string& title, const std::string& content, SDL_Window* window);

        //TODO(Totto):  add option to use the complicated API, that supports more buttons and also a result which button was pressed!
    };


} // namespace helper
