#include "music_manager.hpp"
#include "command_line_arguments.hpp"
#include "constants.hpp"
#include "types.hpp"
#include <SDL.h>
#include <SDL_mixer.h>
#include <filesystem>
#include <fmt/format.h>
#include <functional>
#include <stdexcept>
#include <string>
#include <tl/optional.hpp>

MusicManager::MusicManager(ServiceProvider* service_provider, u8 channel_size)
    : m_music{ nullptr },
      m_queued_music{ nullptr },
      m_channel_size{ channel_size },
      m_chunk_map{ std::unordered_map<std::string, Mix_Chunk*>{} },
      m_service_provider{ service_provider } {
    if (s_instance != nullptr) {
        spdlog::error("it's not allowed to create more than one MusicManager instance");
        return;
    }

    if (m_service_provider->command_line_arguments().silent) {
        return;
    }

    int flags = 0;


#if defined(AUDIO_WITH_FLAC_SUPPORT)
    flags |= MIX_INIT_FLAC;
#endif

#if defined(AUDIO_WITH_MP3_SUPPORT)

    flags |= MIX_INIT_MP3;
#endif
    flags = MIX_INIT_MOD | MIX_INIT_OGG;


    const int result = SDL_InitSubSystem(SDL_INIT_AUDIO);
    if (result != 0) {
        throw std::runtime_error{ "error on initializing the audio system: " + std::string{ SDL_GetError() } };
    }

    int result_flags = Mix_Init(flags);
    if (result_flags != flags) {
        throw std::runtime_error{ fmt::format(
                "error on initializing a certain audio system: expected flags {:#02b} but got {:#02b} - {}", flags,
                result_flags, std::string{ SDL_GetError() }
        ) };
    }
    Mix_AllocateChannels(channel_size);
    // 2 here means STEREO, note that channels above means tracks, e.g. simultaneous playing source that are mixed,
    // hence the name SDL2_mixer
    Mix_OpenAudio(constants::audio_frequency, MIX_DEFAULT_FORMAT, 2, constants::audio_chunk_size);
    s_instance = this;
}

MusicManager::~MusicManager() {
    if (not validate_instance()) {
        return;
    }

    if (m_service_provider->command_line_arguments().silent) {
        return;
    }

    // stop sounds and free loaded data
    Mix_HaltChannel(-1);
    if (m_music != nullptr) {
        Mix_FreeMusic(m_music);
    }
    for (const auto& [_, value] : m_chunk_map) {
        Mix_FreeChunk(value);
    }
    Mix_CloseAudio();
    Mix_Quit();
}

tl::optional<std::string> MusicManager::load_and_play_music(const std::filesystem::path& location, const usize delay) {
    if (not validate_instance()) {
        return tl::nullopt;
    }

    if (m_service_provider->command_line_arguments().silent) {
        spdlog::debug("trying to load and play music \"{}\" in silent mode is ignored", location.string());
        return tl::nullopt;
    }

    Mix_Music* music = Mix_LoadMUS(location.string().c_str());
    if (music == nullptr) {
        return ("an error occurred while trying to load the music '" + location.string()
                + "': " + std::string{ Mix_GetError() });
    }

    if (m_queued_music != nullptr) {
        // if we already have queued a music just que the new one, this could be a potential race condition in a MT case (even if using atomic!)
        m_queued_music = music;
        return tl::nullopt;
    }

    if (m_music != nullptr) {

        if (delay == 0) {
            // the return value is always teh same
            Mix_HaltMusic();
            // it jumps out of these branches into the Mix_PlayMusic call
        } else {

            m_queued_music = music;
            m_delay = delay;

            Mix_HookMusicFinished([]() {
                assert(s_instance != nullptr and "there must be a MusicManager instance");
                s_instance->hook_music_finished();
            });

            // this wasn't block, so we have to wait for the callback to be called
            const int result = Mix_FadeOutMusic(static_cast<int>(delay));
            if (result == 0) {
                return "UNREACHABLE: m_music was not null but not playing, this is an implementation error!";
            }

            return tl::nullopt;
        }
    }

    const int result = Mix_PlayMusic(music, -1);
    if (result != 0) {
        return ("an error occurred while trying to play the music: " + std::string{ Mix_GetError() });
    }
    m_music = music;


    return tl::nullopt;
}

tl::optional<std::string> MusicManager::load_effect(const std::string& name, std::filesystem::path& location) {
    if (not validate_instance()) {
        return tl::nullopt;
    }

    if (m_chunk_map.contains(name)) {
        return "name already used";
    }

    Mix_Chunk* chunk = Mix_LoadWAV(location.string().c_str());
    if (chunk == nullptr) {
        return ("an error occurred while trying to load the chunk: " + std::string{ Mix_GetError() });
    }

    m_chunk_map.insert({ name, chunk });
    return tl::nullopt;
}

tl::optional<std::string> MusicManager::play_effect(const std::string& name, u8 channel_num, int loop) {
    if (not validate_instance()) {
        return tl::nullopt;
    }

    if (m_chunk_map.contains(name)) {
        return "name not loaded";
    }

    if (channel_num <= 0 or channel_num >= m_channel_size) {
        return "invalid channel: " + std::to_string(channel_num);
    }

    auto* const chunk = m_chunk_map.at(name);

    const auto actual_channel = Mix_PlayChannel(channel_num, chunk, loop);
    if (actual_channel == -1) {
        return "couldn't play on channel: " + std::to_string(channel_num);
    }

    return tl::nullopt;
}

void MusicManager::hook_music_finished() {
    if (not validate_instance()) {
        return;
    }

    if (m_queued_music == nullptr) {
        throw std::runtime_error{ "implementation error: m_queued_music is null but it shouldn't be" };
    }

    Mix_FreeMusic(this->m_music);

    const int result = Mix_FadeInMusic(m_queued_music, -1, static_cast<int>(m_delay));
    if (result != 0) {
        throw std::runtime_error(
                "an error occurred while trying to play the music (fading in): " + std::string{ Mix_GetError() }
        );
    }
    this->m_music = m_queued_music;

    m_queued_music = nullptr;

    // clear the callback
    Mix_HookMusicFinished(nullptr);
}

[[nodiscard]] bool MusicManager::validate_instance() {
    if (s_instance != this) {
        if (not m_service_provider->command_line_arguments().silent) {
            spdlog::error("this MusicManager instance is not the instance that is used globally");
        }
        return false;
    }
    return true;
}
