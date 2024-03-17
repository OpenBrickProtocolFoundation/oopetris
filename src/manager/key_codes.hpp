#pragma once

#include <SDL.h>

//TODO: remove this, SDL has it's own conversion function:
// https://wiki.libsdl.org/SDL2/SDL_GetKeyFromName
// https://wiki.libsdl.org/SDL2/SDL_GetKeyName

enum class KeyCode {
    Unknown,
    Return,
    Escape,
    Backspace,
    Tab,
    Space,
    ExclamationMark,
    QuoteDouble,
    Hash,
    Percent,
    Dollar,
    Ampersand,
    Quote,
    LeftParenthesis,
    RightParenthesis,
    Asterisk,
    Plus,
    Comma,
    Minus,
    Period,
    Slash,
    Key0,
    Key1,
    Key2,
    Key3,
    Key4,
    Key5,
    Key6,
    Key7,
    Key8,
    Key9,
    Colon,
    Semicolon,
    Less,
    Equals,
    Greater,
    QuestionMark,
    At,
    LeftBracket,
    Backslash,
    RightBracket,
    Caret,
    Underscore,
    Backquote,
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
    CapsLock,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    PrintScreen,
    ScrollLock,
    Pause,
    Insert,
    Home,
    PageUp,
    Delete,
    End,
    PageDown,
    Right,
    Left,
    Down,
    Up,
    NumLockClear,
    NumPadDivide,
    NumPadMultiply,
    NumPadMinus,
    NumPadPlus,
    NumPadEnter,
    NumPad1,
    NumPad2,
    NumPad3,
    NumPad4,
    NumPad5,
    NumPad6,
    NumPad7,
    NumPad8,
    NumPad9,
    NumPad0,
    NumPadPeriod,
};

inline SDL_KeyCode to_sdl_keycode(KeyCode code) {
    switch (code) {
        case KeyCode::Unknown:
            return SDLK_UNKNOWN;
        case KeyCode::Return:
            return SDLK_RETURN;
        case KeyCode::Escape:
            return SDLK_ESCAPE;
        case KeyCode::Backspace:
            return SDLK_BACKSPACE;
        case KeyCode::Tab:
            return SDLK_TAB;
        case KeyCode::Space:
            return SDLK_SPACE;
        case KeyCode::ExclamationMark:
            return SDLK_EXCLAIM;
        case KeyCode::QuoteDouble:
            return SDLK_QUOTEDBL;
        case KeyCode::Hash:
            return SDLK_HASH;
        case KeyCode::Percent:
            return SDLK_PERCENT;
        case KeyCode::Dollar:
            return SDLK_DOLLAR;
        case KeyCode::Ampersand:
            return SDLK_AMPERSAND;
        case KeyCode::Quote:
            return SDLK_QUOTE;
        case KeyCode::LeftParenthesis:
            return SDLK_LEFTPAREN;
        case KeyCode::RightParenthesis:
            return SDLK_RIGHTPAREN;
        case KeyCode::Asterisk:
            return SDLK_ASTERISK;
        case KeyCode::Plus:
            return SDLK_PLUS;
        case KeyCode::Comma:
            return SDLK_COMMA;
        case KeyCode::Minus:
            return SDLK_MINUS;
        case KeyCode::Period:
            return SDLK_PERIOD;
        case KeyCode::Slash:
            return SDLK_SLASH;
        case KeyCode::Key0:
            return SDLK_0;
        case KeyCode::Key1:
            return SDLK_1;
        case KeyCode::Key2:
            return SDLK_2;
        case KeyCode::Key3:
            return SDLK_3;
        case KeyCode::Key4:
            return SDLK_4;
        case KeyCode::Key5:
            return SDLK_5;
        case KeyCode::Key6:
            return SDLK_6;
        case KeyCode::Key7:
            return SDLK_7;
        case KeyCode::Key8:
            return SDLK_8;
        case KeyCode::Key9:
            return SDLK_9;
        case KeyCode::Colon:
            return SDLK_COLON;
        case KeyCode::Semicolon:
            return SDLK_SEMICOLON;
        case KeyCode::Less:
            return SDLK_LESS;
        case KeyCode::Equals:
            return SDLK_EQUALS;
        case KeyCode::Greater:
            return SDLK_GREATER;
        case KeyCode::QuestionMark:
            return SDLK_QUESTION;
        case KeyCode::At:
            return SDLK_AT;
        case KeyCode::LeftBracket:
            return SDLK_LEFTBRACKET;
        case KeyCode::Backslash:
            return SDLK_BACKSLASH;
        case KeyCode::RightBracket:
            return SDLK_RIGHTBRACKET;
        case KeyCode::Caret:
            return SDLK_CARET;
        case KeyCode::Underscore:
            return SDLK_UNDERSCORE;
        case KeyCode::Backquote:
            return SDLK_BACKQUOTE;
        case KeyCode::A:
            return SDLK_a;
        case KeyCode::B:
            return SDLK_b;
        case KeyCode::C:
            return SDLK_c;
        case KeyCode::D:
            return SDLK_d;
        case KeyCode::E:
            return SDLK_e;
        case KeyCode::F:
            return SDLK_f;
        case KeyCode::G:
            return SDLK_g;
        case KeyCode::H:
            return SDLK_h;
        case KeyCode::I:
            return SDLK_i;
        case KeyCode::J:
            return SDLK_j;
        case KeyCode::K:
            return SDLK_k;
        case KeyCode::L:
            return SDLK_l;
        case KeyCode::M:
            return SDLK_m;
        case KeyCode::N:
            return SDLK_n;
        case KeyCode::O:
            return SDLK_o;
        case KeyCode::P:
            return SDLK_p;
        case KeyCode::Q:
            return SDLK_q;
        case KeyCode::R:
            return SDLK_r;
        case KeyCode::S:
            return SDLK_s;
        case KeyCode::T:
            return SDLK_t;
        case KeyCode::U:
            return SDLK_u;
        case KeyCode::V:
            return SDLK_v;
        case KeyCode::W:
            return SDLK_w;
        case KeyCode::X:
            return SDLK_x;
        case KeyCode::Y:
            return SDLK_y;
        case KeyCode::Z:
            return SDLK_z;
        case KeyCode::CapsLock:
            return SDLK_CAPSLOCK;
        case KeyCode::F1:
            return SDLK_F1;
        case KeyCode::F2:
            return SDLK_F2;
        case KeyCode::F3:
            return SDLK_F3;
        case KeyCode::F4:
            return SDLK_F4;
        case KeyCode::F5:
            return SDLK_F5;
        case KeyCode::F6:
            return SDLK_F6;
        case KeyCode::F7:
            return SDLK_F7;
        case KeyCode::F8:
            return SDLK_F8;
        case KeyCode::F9:
            return SDLK_F9;
        case KeyCode::F10:
            return SDLK_F10;
        case KeyCode::F11:
            return SDLK_F11;
        case KeyCode::F12:
            return SDLK_F12;
        case KeyCode::PrintScreen:
            return SDLK_PRINTSCREEN;
        case KeyCode::ScrollLock:
            return SDLK_SCROLLLOCK;
        case KeyCode::Pause:
            return SDLK_PAUSE;
        case KeyCode::Insert:
            return SDLK_INSERT;
        case KeyCode::Home:
            return SDLK_HOME;
        case KeyCode::PageUp:
            return SDLK_PAGEUP;
        case KeyCode::Delete:
            return SDLK_DELETE;
        case KeyCode::End:
            return SDLK_END;
        case KeyCode::PageDown:
            return SDLK_PAGEDOWN;
        case KeyCode::Right:
            return SDLK_RIGHT;
        case KeyCode::Left:
            return SDLK_LEFT;
        case KeyCode::Down:
            return SDLK_DOWN;
        case KeyCode::Up:
            return SDLK_UP;
        case KeyCode::NumLockClear:
            return SDLK_NUMLOCKCLEAR;
        case KeyCode::NumPadDivide:
            return SDLK_KP_DIVIDE;
        case KeyCode::NumPadMultiply:
            return SDLK_KP_MULTIPLY;
        case KeyCode::NumPadMinus:
            return SDLK_KP_MINUS;
        case KeyCode::NumPadPlus:
            return SDLK_KP_PLUS;
        case KeyCode::NumPadEnter:
            return SDLK_KP_ENTER;
        case KeyCode::NumPad1:
            return SDLK_KP_1;
        case KeyCode::NumPad2:
            return SDLK_KP_2;
        case KeyCode::NumPad3:
            return SDLK_KP_3;
        case KeyCode::NumPad4:
            return SDLK_KP_4;
        case KeyCode::NumPad5:
            return SDLK_KP_5;
        case KeyCode::NumPad6:
            return SDLK_KP_6;
        case KeyCode::NumPad7:
            return SDLK_KP_7;
        case KeyCode::NumPad8:
            return SDLK_KP_8;
        case KeyCode::NumPad9:
            return SDLK_KP_9;
        case KeyCode::NumPad0:
            return SDLK_KP_0;
        case KeyCode::NumPadPeriod:
            return SDLK_KP_PERIOD;
        default:
            return SDLK_UNKNOWN;
    }
}

inline KeyCode from_sdl_keycode(SDL_KeyCode code) {
    switch (code) {
        case SDLK_UNKNOWN:
            return KeyCode::Unknown;
        case SDLK_RETURN:
            return KeyCode::Return;
        case SDLK_ESCAPE:
            return KeyCode::Escape;
        case SDLK_BACKSPACE:
            return KeyCode::Backspace;
        case SDLK_TAB:
            return KeyCode::Tab;
        case SDLK_SPACE:
            return KeyCode::Space;
        case SDLK_EXCLAIM:
            return KeyCode::ExclamationMark;
        case SDLK_QUOTEDBL:
            return KeyCode::QuoteDouble;
        case SDLK_HASH:
            return KeyCode::Hash;
        case SDLK_PERCENT:
            return KeyCode::Percent;
        case SDLK_DOLLAR:
            return KeyCode::Dollar;
        case SDLK_AMPERSAND:
            return KeyCode::Ampersand;
        case SDLK_QUOTE:
            return KeyCode::Quote;
        case SDLK_LEFTPAREN:
            return KeyCode::LeftParenthesis;
        case SDLK_RIGHTPAREN:
            return KeyCode::RightParenthesis;
        case SDLK_ASTERISK:
            return KeyCode::Asterisk;
        case SDLK_PLUS:
            return KeyCode::Plus;
        case SDLK_COMMA:
            return KeyCode::Comma;
        case SDLK_MINUS:
            return KeyCode::Minus;
        case SDLK_PERIOD:
            return KeyCode::Period;
        case SDLK_SLASH:
            return KeyCode::Slash;
        case SDLK_0:
            return KeyCode::Key0;
        case SDLK_1:
            return KeyCode::Key1;
        case SDLK_2:
            return KeyCode::Key2;
        case SDLK_3:
            return KeyCode::Key3;
        case SDLK_4:
            return KeyCode::Key4;
        case SDLK_5:
            return KeyCode::Key5;
        case SDLK_6:
            return KeyCode::Key6;
        case SDLK_7:
            return KeyCode::Key7;
        case SDLK_8:
            return KeyCode::Key8;
        case SDLK_9:
            return KeyCode::Key9;
        case SDLK_COLON:
            return KeyCode::Colon;
        case SDLK_SEMICOLON:
            return KeyCode::Semicolon;
        case SDLK_LESS:
            return KeyCode::Less;
        case SDLK_EQUALS:
            return KeyCode::Equals;
        case SDLK_GREATER:
            return KeyCode::Greater;
        case SDLK_QUESTION:
            return KeyCode::QuestionMark;
        case SDLK_AT:
            return KeyCode::At;
        case SDLK_LEFTBRACKET:
            return KeyCode::LeftBracket;
        case SDLK_BACKSLASH:
            return KeyCode::Backslash;
        case SDLK_RIGHTBRACKET:
            return KeyCode::RightBracket;
        case SDLK_CARET:
            return KeyCode::Caret;
        case SDLK_UNDERSCORE:
            return KeyCode::Underscore;
        case SDLK_BACKQUOTE:
            return KeyCode::Backquote;
        case SDLK_a:
            return KeyCode::A;
        case SDLK_b:
            return KeyCode::B;
        case SDLK_c:
            return KeyCode::C;
        case SDLK_d:
            return KeyCode::D;
        case SDLK_e:
            return KeyCode::E;
        case SDLK_f:
            return KeyCode::F;
        case SDLK_g:
            return KeyCode::G;
        case SDLK_h:
            return KeyCode::H;
        case SDLK_i:
            return KeyCode::I;
        case SDLK_j:
            return KeyCode::J;
        case SDLK_k:
            return KeyCode::K;
        case SDLK_l:
            return KeyCode::L;
        case SDLK_m:
            return KeyCode::M;
        case SDLK_n:
            return KeyCode::N;
        case SDLK_o:
            return KeyCode::O;
        case SDLK_p:
            return KeyCode::P;
        case SDLK_q:
            return KeyCode::Q;
        case SDLK_r:
            return KeyCode::R;
        case SDLK_s:
            return KeyCode::S;
        case SDLK_t:
            return KeyCode::T;
        case SDLK_u:
            return KeyCode::U;
        case SDLK_v:
            return KeyCode::V;
        case SDLK_w:
            return KeyCode::W;
        case SDLK_x:
            return KeyCode::X;
        case SDLK_y:
            return KeyCode::Y;
        case SDLK_z:
            return KeyCode::Z;
        case SDLK_CAPSLOCK:
            return KeyCode::CapsLock;
        case SDLK_F1:
            return KeyCode::F1;
        case SDLK_F2:
            return KeyCode::F2;
        case SDLK_F3:
            return KeyCode::F3;
        case SDLK_F4:
            return KeyCode::F4;
        case SDLK_F5:
            return KeyCode::F5;
        case SDLK_F6:
            return KeyCode::F6;
        case SDLK_F7:
            return KeyCode::F7;
        case SDLK_F8:
            return KeyCode::F8;
        case SDLK_F9:
            return KeyCode::F9;
        case SDLK_F10:
            return KeyCode::F10;
        case SDLK_F11:
            return KeyCode::F11;
        case SDLK_F12:
            return KeyCode::F12;
        case SDLK_PRINTSCREEN:
            return KeyCode::PrintScreen;
        case SDLK_SCROLLLOCK:
            return KeyCode::ScrollLock;
        case SDLK_PAUSE:
            return KeyCode::Pause;
        case SDLK_INSERT:
            return KeyCode::Insert;
        case SDLK_HOME:
            return KeyCode::Home;
        case SDLK_PAGEUP:
            return KeyCode::PageUp;
        case SDLK_DELETE:
            return KeyCode::Delete;
        case SDLK_END:
            return KeyCode::End;
        case SDLK_PAGEDOWN:
            return KeyCode::PageDown;
        case SDLK_RIGHT:
            return KeyCode::Right;
        case SDLK_LEFT:
            return KeyCode::Left;
        case SDLK_DOWN:
            return KeyCode::Down;
        case SDLK_UP:
            return KeyCode::Up;
        case SDLK_NUMLOCKCLEAR:
            return KeyCode::NumLockClear;
        case SDLK_KP_DIVIDE:
            return KeyCode::NumPadDivide;
        case SDLK_KP_MULTIPLY:
            return KeyCode::NumPadMultiply;
        case SDLK_KP_MINUS:
            return KeyCode::NumPadMinus;
        case SDLK_KP_PLUS:
            return KeyCode::NumPadPlus;
        case SDLK_KP_ENTER:
            return KeyCode::NumPadEnter;
        case SDLK_KP_1:
            return KeyCode::NumPad1;
        case SDLK_KP_2:
            return KeyCode::NumPad2;
        case SDLK_KP_3:
            return KeyCode::NumPad3;
        case SDLK_KP_4:
            return KeyCode::NumPad4;
        case SDLK_KP_5:
            return KeyCode::NumPad5;
        case SDLK_KP_6:
            return KeyCode::NumPad6;
        case SDLK_KP_7:
            return KeyCode::NumPad7;
        case SDLK_KP_8:
            return KeyCode::NumPad8;
        case SDLK_KP_9:
            return KeyCode::NumPad9;
        case SDLK_KP_0:
            return KeyCode::NumPad0;
        case SDLK_KP_PERIOD:
            return KeyCode::NumPadPeriod;
        default:
            return KeyCode::Unknown;
    }
}
