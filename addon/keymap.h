#ifndef _FCITX5_VNKEY_KEYMAP_H_
#define _FCITX5_VNKEY_KEYMAP_H_

// addon/keymap.h
// Dịch fcitx::Key -> engine::KeyInput. Đây là ranh giới duy nhất nơi kiểu phím
// của Fcitx5 được chuyển sang kiểu phím thuần của core.

#include <fcitx-utils/key.h>

#include "keyinput.hpp"

namespace fcitx {
    engine::KeyInput toKeyInput(const Key &key);
} // namespace fcitx

#endif  // _FCITX5_VNKEY_KEYMAP_H_
