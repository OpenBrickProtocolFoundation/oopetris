#pragma once

#include "key_codes.hpp"
#include "recording.hpp"

struct KeyboardControls {
    KeyCode rotate_left = KeyCode::Left;
    KeyCode rotate_right = KeyCode::Right;
    KeyCode move_left = KeyCode::A;
    KeyCode move_right = KeyCode::D;
    KeyCode move_down = KeyCode::S;
    KeyCode drop = KeyCode::W;
    KeyCode hold = KeyCode::Tab;
};

struct ReplayControls {
    RecordingReader recording;

    explicit ReplayControls(RecordingReader recording) : recording{ std::move(recording) } { }
};
