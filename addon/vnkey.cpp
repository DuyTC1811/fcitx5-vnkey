// addon/vnkey.cpp
#include "vnkey.h"

#include <string>
#include <fcitx-utils/keysym.h>
#include <fcitx/inputpanel.h>

#include "keymap.h"

namespace fcitx {
    VnKeyEngine::VnKeyEngine(Instance *instance)
        : instance_(instance),
          factory_([](InputContext &ic) { return new VnKeyState(&ic); }) {
        instance_->inputContextManager().registerProperty("vnkeyState", &factory_);
    }

    void VnKeyEngine::keyEvent(const InputMethodEntry & /*entry*/, KeyEvent &keyEvent) {
        if (keyEvent.isRelease()) {
            return;
        }

        InputContext *ic = keyEvent.inputContext();
        auto *state = ic->propertyFor(&factory_);

        // Dịch phím Fcitx5 -> phím thuần của core, rồi để core quyết định.
        const engine::KeyInput ki = toKeyInput(keyEvent.key());
        const engine::Result result = state->processor().process(ki);

        switch (result.action) {
            case engine::Action::PASS_THROUGH:
                // Core không xử lý -> không filter, ứng dụng nhận phím như thường.
                return;

            case engine::Action::UPDATE_PREEDIT:
                showPreedit(ic, result.text);
                keyEvent.filterAndAccept();
                return;

            case engine::Action::COMMIT:
                ic->commitString(result.text);
                clearPreedit(ic);
                // Với phím space/enter: đã commit từ, giờ để phím trắng đó tới app
                // (không filter) -> "nặng" + dấu cách. Các phím khác thì nuốt luôn.
                if (ki.special == engine::KeyInput::Special::SPACE ||
                    ki.special == engine::KeyInput::Special::ENTER) {
                    return; // unfiltered
                }
                keyEvent.filterAndAccept();
                return;

            case engine::Action::COMMIT_WITH_BACKSPACE: {
                // Xóa `backspaces` ký tự đã commit trước đó (fake-backspace) rồi
                // commit chuỗi mới. Dùng khi không đi theo mô hình preedit.
                for (int i = 0; i < result.backspaces; ++i) {
                    ic->forwardKey(Key(FcitxKey_BackSpace), false);
                    ic->forwardKey(Key(FcitxKey_BackSpace), true);
                }
                ic->commitString(result.text);
                clearPreedit(ic);
                keyEvent.filterAndAccept();
                return;
            }
        }
    }

    void VnKeyEngine::showPreedit(InputContext *ic, const std::string &text) {
        Text preedit;
        preedit.append(text, TextFormatFlag::Underline);
        preedit.setCursor(static_cast<int>(text.size()));

        // setClientPreedit = hiển thị ngay tại con trỏ (on-the-spot) nếu app hỗ trợ.
        if (ic->capabilityFlags().test(CapabilityFlag::Preedit)) {
            ic->inputPanel().setClientPreedit(preedit);
        }
        ic->inputPanel().setPreedit(preedit);
        ic->updatePreedit();
        ic->updateUserInterface(UserInterfaceComponent::InputPanel);
    }

    void VnKeyEngine::clearPreedit(InputContext *ic) {
        ic->inputPanel().reset();
        ic->updatePreedit();
        ic->updateUserInterface(UserInterfaceComponent::InputPanel);
    }

    void VnKeyEngine::reset(const InputMethodEntry & /*entry*/, InputContextEvent &event) {
        InputContext *ic = event.inputContext();
        ic->propertyFor(&factory_)->processor().reset();
        clearPreedit(ic);
    }
} // namespace fcitx

FCITX_ADDON_FACTORY(fcitx::VnKeyEngineFactory);
