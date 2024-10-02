#pragma once


#include <core/helper/types.hpp>

#include "helper/windows.hpp"
#include "input/input.hpp"
#include "manager/service_provider.hpp"

#include <SDL.h>
#include <SDL_mixer.h>
#include <atomic>
#include <filesystem>
#include <functional>
#include <string>
#include <unordered_map>

// a general note on music vs. chunk: a chunk is loaded into memory and there may be many of them (effects, not to large music)
// a music is unique and is decoded on the fly, so there is only one of them at the time!

struct MusicManager final {
private:
    static inline MusicManager* s_instance{ nullptr }; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
    static const constexpr double step_width = 0.05F;

    using VolumeChangeFunction = std::function<void(std::optional<double> volume)>;

    Mix_Music* m_music;
    std::atomic<Mix_Music*> m_queued_music;
    u8 m_channel_size;
    std::unordered_map<std::string, Mix_Chunk*> m_chunk_map;
    static constexpr unsigned fade_ms = 500;
    usize m_delay = MusicManager::fade_ms;

    std::optional<double> m_volume;
    std::unordered_map<std::string, VolumeChangeFunction> m_volume_listeners;

public:
    OOPETRIS_GRAPHICS_EXPORTED explicit MusicManager(ServiceProvider* service_provider, u8 channel_size = 2);

    OOPETRIS_GRAPHICS_EXPORTED MusicManager(const MusicManager&) = delete;
    OOPETRIS_GRAPHICS_EXPORTED MusicManager& operator=(const MusicManager&) = delete;

    OOPETRIS_GRAPHICS_EXPORTED MusicManager(const MusicManager&&) = delete;
    OOPETRIS_GRAPHICS_EXPORTED MusicManager& operator=(MusicManager&&) = delete;

    OOPETRIS_GRAPHICS_EXPORTED ~MusicManager() noexcept;

    OOPETRIS_GRAPHICS_EXPORTED std::optional<std::string>
    load_and_play_music(const std::filesystem::path& location, usize delay = MusicManager::fade_ms);

    OOPETRIS_GRAPHICS_EXPORTED std::optional<std::string>
    load_effect(const std::string& name, std::filesystem::path& location);
    OOPETRIS_GRAPHICS_EXPORTED std::optional<std::string>
    play_effect(const std::string& name, u8 channel_num = 1, int loop = 0);

    //TODO(Totto):  atm the volume changers only work on the music channel, when adding more effects, this should support channels via  https://wiki.libsdl.org/SDL2_mixer/Mix_Volume
    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::optional<double> get_volume() const;

    OOPETRIS_GRAPHICS_EXPORTED void
    set_volume(std::optional<double> new_volume, bool force_update = false, bool notify_listeners = true);

    // no nodiscard, since the return value is only a side effect, that is maybe useful
    OOPETRIS_GRAPHICS_EXPORTED std::optional<double> change_volume(std::int8_t steps);

    OOPETRIS_GRAPHICS_EXPORTED bool
    handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event);

    bool add_volume_listener(const std::string& name, VolumeChangeFunction change_function) {

        if (m_volume_listeners.contains(name)) {
            return false;
        }

        m_volume_listeners.insert_or_assign(name, std::move(change_function));
        return true;
    }

    bool remove_volume_listener(const std::string& name) {
        if (not m_volume_listeners.contains(name)) {
            return false;
        }

        m_volume_listeners.erase(name);
        return true;
    }

private:
    void hook_music_finished();
    [[nodiscard]] bool validate_instance();
};
