#pragma once

#include "types.hpp"
#include <SDL_mixer.h>
#include <filesystem>
#include <string>
#include <tl/optional.hpp>
#include <unordered_map>

// a general note on music vs. chunk: a chunk is loaded into memory and there may be many of them (effects, not to large music)
// a music is unique and is decoded on the fly, so there is only one of them at the time!

struct MusicManager {
private:
    Mix_Music* m_music;
    u8 m_channel_size;
    std::unordered_map<std::string, Mix_Chunk*> m_chunk_map;
    static constexpr unsigned fade_ms = 500;


public:
    MusicManager(u8 channel_size = 2);
    MusicManager(const MusicManager&) = delete;
    MusicManager& operator=(const MusicManager&) = delete;
    ~MusicManager();

    tl::optional<std::string> load_and_play_music(const std::filesystem::path& location);

    tl::optional<std::string> load_effect(const std::string& name, std::filesystem::path& location);
    tl::optional<std::string> play_effect(const std::string& name, u8 channel_num = 1, int loop = 0);
};
