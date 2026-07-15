#pragma once
namespace engine {
    // Kieu phim rieng cua core — KHONG dung fcitx::Key.
    // addon/src/keymap.cpp chiu trach nhiem dich fcitx::Key -> KeyInput.
    struct KeyInput {
        enum class Special {
            NONE,
            BACKSPACE,
            ESCAPE,
            SPACE,
            ENTER,
            DELETE,
            TAB
        };

        char32_t ch = 0; // ky tu unicode, 0 neu la phim dieu khien
        Special special = Special::NONE;
        bool shift = false;
        bool ctrlOrAlt = false; // co modifier -> thuong la passthrough
    };
} // END engine
