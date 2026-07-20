// addon/keymap.h
// Dịch fcitx::Key -> engine::KeyInput. Đây là ranh giới duy nhất nơi kiểu phím
// của Fcitx5 được chuyển sang kiểu phím thuần của core.
#pragma once

#include <fcitx-utils/key.h>

#include "keyinput.hpp"

namespace fcitx {
    engine::KeyInput toKeyInput(const Key& key);
} // namespace fcitx
