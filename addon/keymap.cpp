// addon/keymap.cpp
#include "keymap.h"

#include <fcitx-utils/keysym.h>

namespace fcitx {
    engine::KeyInput toKeyInput(const Key &key) {
        using Special = engine::KeyInput::Special;

        engine::KeyInput out;

        const KeyStates states = key.states();
        out.shift = states.test(KeyState::Shift);
        // Bất kỳ Ctrl/Alt/Super nào -> core sẽ coi là passthrough.
        out.ctrlOrAlt = states.test(KeyState::Ctrl) || states.test(KeyState::Alt) ||
                        states.test(KeyState::Super);

        switch (key.sym()) {
            case FcitxKey_BackSpace:
                out.special = Special::BACKSPACE;
                return out;
            case FcitxKey_Escape:
                out.special = Special::ESCAPE;
                return out;
            case FcitxKey_space:
                out.special = Special::SPACE;
                return out;
            case FcitxKey_Return:
            case FcitxKey_KP_Enter:
                out.special = Special::ENTER;
                return out;
            case FcitxKey_Delete:
                out.special = Special::DELETE;
                return out;
            case FcitxKey_Tab:
                out.special = Special::TAB;
                return out;
            default:
                break;
        }

        // Ký tự in được -> lấy codepoint Unicode. Trả 0 nếu không phải ký tự
        // (phím mũi tên, F1...), core sẽ passthrough.
        out.ch = Key::keySymToUnicode(key.sym());
        return out;
    }
} // namespace fcitx
