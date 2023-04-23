#include "music_manager.hpp"
#include "types.hpp"
#include <SDL.h>
#include <SDL_mixer.h>
#include <filesystem>
#include <functional>
#include <stdexcept>
#include <string>
#include <tl/optional.hpp>

std::unordered_map<std::string, Mix_Chunk*> m_chunk_map;
MusicManager::MusicManager(u8 channel_size)
    : m_channel_size{ channel_size },
      m_chunk_map{ std::unordered_map<std::string, Mix_Chunk*>{} } {
    Mix_Init(MIX_INIT_FLAC | MIX_INIT_MP3);
    SDL_InitSubSystem(SDL_INIT_AUDIO);
    Mix_AllocateChannels(channel_size);
    // 2 here means STEREO, note that channels above means tracks, e.g simultaneous playing source that are mixed, hence the name SDL2_mixer)
    Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 2, 4096);
}


tl::optional<std::string> MusicManager::load_and_play_music(const std::filesystem::path& location) {


    Mix_Music* music = Mix_LoadMUS(location.string().c_str());
    if (!music) {
        return ("an error occurred while trying to load the music: " + std::string{ Mix_GetError() });
    }

    if (m_music) {

        std::function<void()> music_finished = [&]() {
            //after it stopped, use async

            Mix_FreeMusic(m_music);


            int result = Mix_FadeInMusic(music, -1, MusicManager::fade_ms);
            if (result != 0) {
                throw std::runtime_error(
                        "an error occurred while trying to play the music (fading in): " + std::string{ Mix_GetError() }
                );
            }
            m_music = music;

            // clear the callback
            Mix_HookMusicFinished(nullptr);
        };

        //TODO test if this works correctly
        Mix_HookMusicFinished(music_finished.target<void()>());

        // this wan't block, so we have to wait for the callback to be called
        int result = Mix_FadeOutMusic(MusicManager::fade_ms);
        if (result == 0) {
            return "UNREACHABLE: m_music was not null but no playing, this is an implementation error!";
        }


    } else {

        int result = Mix_PlayMusic(music, -1);
        if (result != 0) {
            return ("an error occurred while trying to play the music: " + std::string{ Mix_GetError() });
        }
        m_music = music;
    }


    return tl::nullopt;
}


MusicManager::~MusicManager() {

    // stop sounds and free loaded data
    Mix_HaltChannel(-1);
    if (m_music) {
        Mix_FreeMusic(m_music);
    }
    //TODO
    /*  for (snd = 0; snd < 4; snd++)
        if (sound[snd])
            Mix_FreeChunk(sound[snd]); */
    Mix_CloseAudio();
    Mix_Quit();
}
