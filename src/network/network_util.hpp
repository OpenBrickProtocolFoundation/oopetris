

#pragma once

#include "string"
#include <SDL_net.h>
#include <cstring>

namespace network_util {


    inline std::string latest_sdl_net_error() {


        const char* sdl_err = SDLNet_GetError();
        const std::string error_message =
                sdl_err == nullptr || std::strlen(sdl_err) == 0 ? "Unknown SDL_net error" : std::string{ sdl_err };

        return error_message;
    }


} // namespace network_util