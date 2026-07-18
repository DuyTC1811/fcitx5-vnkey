#include "vnkey.h"
#include "keymap.h" // fcitx::toKeyInput(const Key&) -> engine::KeyInput

#include <fcitx-utils/key.h>
#include <fcitx/inputcontext.h>
#include <fcitx/inputpanel.h>

using namespace fcitx;

namespace {
    // Hien thi preedit (chu dang go do). Uu tien client preedit de chu
    // hien inline ngay trong app; app nao khong ho tro thi fallback.
    void showPreedit(InputContext* ic, const std::string& s) {
        Text preedit;
        preedit.append(s);
        preedit.setCursor(s.size());
        if (ic->capabilityFlags().test(CapabilityFlag::Preedit)) {
            ic->inputPanel().setClientPreedit(preedit);
        } else {
            ic->inputPanel().setPreedit(preedit);
        }
        ic->updatePreedit();
    }

    void clearPreedit(InputContext* ic) {
        ic->inputPanel().reset();
        ic->updatePreedit();
    }
} // namespace

VnKeyEngine::VnKeyEngine(Instance* instance) :
    instance_(instance), factory_([](InputContext&) { return new VnKeyState(); }) {
    instance_->inputContextManager().registerProperty("vnkeyState", &factory_);
}

void VnKeyEngine::activate(const InputMethodEntry&, InputContextEvent& event) {
    auto* ic = event.inputContext();
    ic->propertyFor(&factory_)->processor_.reset();
}

void VnKeyEngine::reset(const InputMethodEntry&, InputContextEvent& event) {
    auto* ic = event.inputContext();
    ic->propertyFor(&factory_)->processor_.reset();
    clearPreedit(ic);
}

void VnKeyEngine::keyEvent(const InputMethodEntry&, KeyEvent& keyEvent) {
    if (keyEvent.isRelease()) {
        return;
    }

    auto* ic = keyEvent.inputContext();
    auto* state = ic->propertyFor(&factory_);

    // Ranh gioi addon -> core: dich phim Fcitx5 sang kieu thuan cua core.
    engine::KeyInput ki = toKeyInput(keyEvent.key());
    engine::Result r = state->processor_.process(ki);

    switch (r.action) {
        case engine::Action::PASS_THROUGH:
            // Core khong xu ly phim nay -> khong accept, de Fcitx5 forward cho app.
            return;

        case engine::Action::UPDATE_PREEDIT:
            showPreedit(ic, r.text);
            keyEvent.filterAndAccept();
            return;

        case engine::Action::COMMIT:
            clearPreedit(ic);
            ic->commitString(r.text);
            if (r.forwardKey) {
                // KHONG filterAndAccept -> Fcitx5 forward phim (Enter/Tab) cho app
                return;
            }
            keyEvent.filterAndAccept();
            return;

        case engine::Action::COMMIT_WITH_BACKSPACE:
            // Gui r.backspaces phim Backspace de xoa ky tu da commit truoc do,
            // roi commit chuoi moi (fake-backspace).
            for (int i = 0; i < r.backspaces; ++i) {
                ic->forwardKey(Key(FcitxKey_BackSpace), false);
                ic->forwardKey(Key(FcitxKey_BackSpace), true);
            }
            clearPreedit(ic);
            ic->commitString(r.text);
            keyEvent.filterAndAccept();
            return;
    }
}

FCITX_ADDON_FACTORY(VnKeyEngineFactory);
