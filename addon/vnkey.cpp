// addon/vnkey.cpp
#include "vnkey.h"

#include <fcitx-utils/keysymgen.h>
#include <fcitx/inputpanel.h>

#include <string>

namespace fcitx {

VnKeyEngine::VnKeyEngine(Instance* instance)
    : instance_(instance),
      factory_([](InputContext& ic) { return new VnKeyState(&ic); })
{
    instance_->inputContextManager().registerProperty("vnkeyState", &factory_);
}

void VnKeyEngine::keyEvent(const InputMethodEntry& /*entry*/, KeyEvent& keyEvent)
{
    // Ignore key-release events; we only act on press.
    if (keyEvent.isRelease()) {
        return;
    }

    InputContext* ic = keyEvent.inputContext();
    auto* state = ic->propertyFor(&factory_);
    auto& proc = state->processor();

    const Key key = keyEvent.key();

    // --- Commit triggers: space / enter commit the current buffer, then let
    // the key pass through to the application normally. ---
    if (key.check(FcitxKey_space) || key.check(FcitxKey_Return) ||
        key.check(FcitxKey_KP_Enter)) {
        commitBuffer(ic, state);
        return;  // not filtered -> app receives the space/enter itself
    }

    // --- Escape: drop the composing buffer. ---
    if (key.check(FcitxKey_Escape)) {
        proc.reset();  // TODO: match your reset method name
        updatePreedit(ic, state);
        return;
    }

    // --- Backspace: operate on syllable structure inside the processor. ---
    if (key.check(FcitxKey_BackSpace)) {
        // TODO: match your API. Should return true if the buffer consumed the
        // backspace (i.e. there was something to delete), false to pass through.
        bool consumed = proc.backspace();
        if (consumed) {
            updatePreedit(ic, state);
            keyEvent.filterAndAccept();
        }
        return;  // if not consumed, let the app handle backspace natively
    }

    // --- Delete: only consume when there is actually a tone to remove
    //     (matches the established Delete-key behaviour). ---
    if (key.check(FcitxKey_Delete)) {
        bool consumed = proc.deleteTone();  // TODO: match your API name
        if (consumed) {
            updatePreedit(ic, state);
            keyEvent.filterAndAccept();
        }
        return;
    }

    // --- Normal printable ASCII letters feed the Telex processor. ---
    uint32_t sym = key.sym();
    if (sym >= FcitxKey_a && sym <= FcitxKey_z) {
        char c = static_cast<char>('a' + (sym - FcitxKey_a));
        // TODO: match your feed method. It should return whether the key was
        // consumed by the IME (true) or should pass through (false).
        bool consumed = proc.processKey(c);
        if (consumed) {
            updatePreedit(ic, state);
            keyEvent.filterAndAccept();
        }
        return;
    }
    if (sym >= FcitxKey_A && sym <= FcitxKey_Z) {
        char c = static_cast<char>('A' + (sym - FcitxKey_A));
        bool consumed = proc.processKey(c);
        if (consumed) {
            updatePreedit(ic, state);
            keyEvent.filterAndAccept();
        }
        return;
    }

    // Any other key (punctuation, arrows, etc.): commit what we have and let
    // it through, so buffer state stays consistent with the cursor.
    if (!proc.empty()) {  // TODO: match your "is buffer empty" query
        commitBuffer(ic, state);
    }
}

void VnKeyEngine::updatePreedit(InputContext* ic, VnKeyState* state)
{
    // TODO: match your API — should return the current composed string
    // (with tone marks applied) for display, e.g. "nặng".
    std::string composed = state->processor().preedit();

    Text preedit;
    preedit.append(composed, TextFormatFlag::Underline);
    preedit.setCursor(static_cast<int>(composed.size()));

    ic->inputPanel().setClientPreedit(preedit);
    ic->inputPanel().setPreedit(preedit);
    ic->updatePreedit();
}

void VnKeyEngine::commitBuffer(InputContext* ic, VnKeyState* state)
{
    std::string composed = state->processor().preedit();  // TODO: match API
    if (!composed.empty()) {
        ic->commitString(composed);
    }
    state->processor().reset();  // TODO: match API

    // Clear the on-screen preedit.
    ic->inputPanel().reset();
    ic->updatePreedit();
}

void VnKeyEngine::reset(const InputMethodEntry& /*entry*/, InputContextEvent& event)
{
    auto* state = event.inputContext()->propertyFor(&factory_);
    state->processor().reset();  // TODO: match API
    event.inputContext()->inputPanel().reset();
    event.inputContext()->updatePreedit();
}

}  // namespace fcitx

FCITX_ADDON_FACTORY(fcitx::VnKeyEngineFactory);