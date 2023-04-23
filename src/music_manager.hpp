#pragma once

#include "types.hpp"
#include <SDL_mixer.h>
#include <atomic>
#include <filesystem>
#include <string>
#include <tl/optional.hpp>
#include <unordered_map>

// a general note on music vs. chunk: a chunk is loaded into memory and there may be many of them (effects, not to large music)
// a music is unique and is decoded on the fly, so there is only one of them at the time!


// this has to be a singleton because of some static functions and members, that have to be unique (globally!) (an we are in OOP, and do not have a single Singelton yet?!?!?  xD)
struct MusicManager {
private:
    Mix_Music* m_music;
    std::atomic<Mix_Music*> m_queued_music;
    u8 m_channel_size;
    std::unordered_map<std::string, Mix_Chunk*> m_chunk_map;
    static constexpr unsigned fade_ms = 500;
    usize m_delay = MusicManager::fade_ms;
    void hook_music_finished();

    MusicManager(u8 channel_size);

public:
    static MusicManager& getInstance(u8 channel_size = 2);
    MusicManager(const MusicManager&) = delete;
    MusicManager& operator=(const MusicManager&) = delete;
    ~MusicManager();

    tl::optional<std::string>
    load_and_play_music(const std::filesystem::path& location, const usize delay = MusicManager::fade_ms);

    tl::optional<std::string> load_effect(const std::string& name, std::filesystem::path& location);
    tl::optional<std::string> play_effect(const std::string& name, u8 channel_num = 1, int loop = 0);
};
