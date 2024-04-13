

#pragma once

#include <SDL.h>
#include <string>


namespace helper {

#ifdef _WIN32
    // "WinUser.h" defines "#define MessageBox  MessageBoxA"
    // which breaks this valid code, I love windows xD.
#ifdef MessageBox
#undef MessageBox
#endif
#endif


    struct MessageBox {
        enum class Type { Error, Warning, Information };

        /***
        * \brief This blocks the current thread and should be called on the thread, the parent windows was created (if that is nullptr, it may be on another thread, but not otherwise)
        */
        static void show_simple(Type type, const std::string& title, const std::string& content, SDL_Window* window);

        //TODO: add option to use the complicated API, that supports more buttons and also a result which button was pressed!
    };


} // namespace helper
