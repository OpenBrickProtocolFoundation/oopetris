#pragma once

#include "capabilities.hpp"
#include "service_provider.hpp"
#include "types.hpp"
#include <SDL_mixer.h>
#include <atomic>
#include <filesystem>
#include <string>
#include <tl/optional.hpp>
#include <unordered_map>

// a general note on music vs. chunk: a chunk is loaded into memory and there may be many of them (effects, not to large music)
// a music is unique and is decoded on the fly, so there is only one of them at the time!

struct MusicManager final {
private:
    static inline MusicManager* s_instance{ nullptr }; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
    static const inline float step_width = 0.05F;

    using VolumeChangeFunction = std::function<void(tl::optional<float> volume)>;

    Mix_Music* m_music;
    std::atomic<Mix_Music*> m_queued_music;
    u8 m_channel_size;
    std::unordered_map<std::string, Mix_Chunk*> m_chunk_map;
    static constexpr unsigned fade_ms = 500;
    usize m_delay = MusicManager::fade_ms;
    ServiceProvider* m_service_provider;
    tl::optional<float> volume;
    std::unordered_map<std::string, VolumeChangeFunction> volume_listeners;

public:
    explicit MusicManager(ServiceProvider* service_provider, u8 channel_size = 2);
    MusicManager(const MusicManager&) = delete;
    MusicManager& operator=(const MusicManager&) = delete;
    MusicManager(const MusicManager&&) = delete;
    MusicManager& operator=(MusicManager&&) = delete;
    ~MusicManager();

    tl::optional<std::string>
    load_and_play_music(const std::filesystem::path& location, usize delay = MusicManager::fade_ms);

    tl::optional<std::string> load_effect(const std::string& name, std::filesystem::path& location);
    tl::optional<std::string> play_effect(const std::string& name, u8 channel_num = 1, int loop = 0);

    [[nodiscard]] tl::optional<float> get_volume() const;
    void set_volume(tl::optional<float> new_volume, bool force_update = false, bool notify_listeners = true);
    // no nodiscard, since the return value is only a side effect, that is maybe useful
    tl::optional<float> change_volume(std::int8_t steps);

    bool handle_event(const SDL_Event& event);

    bool add_volume_listener(const std::string& name, VolumeChangeFunction change_function) {

        if (volume_listeners.contains(name)) {
            return false;
        }

        volume_listeners.insert_or_assign(name, std::move(change_function));
        return true;
    }

    bool remove_volume_listener(const std::string& name) {
        if (not volume_listeners.contains(name)) {
            return false;
        }

        volume_listeners.erase(name);
        return true;
    }

private:
    void hook_music_finished();
    [[nodiscard]] bool validate_instance();
};
