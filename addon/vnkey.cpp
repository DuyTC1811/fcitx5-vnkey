#include "vnkey.h"

#include <fcitx/inputcontext.h>
#include <fcitx/inputpanel.h>
#include <fcitx-utils/keysymgen.h>

#include "engine/input_processor.h"
#include "engine/key_event.h"   // struct nhẹ của core, độc lập Fcitx5

using namespace fcitx;

namespace {

// Chuyển fcitx::Key sang kiểu key event riêng của core, để core
// không bao giờ phải include <fcitx/...>. Đây là "ranh giới" giữa
// hai layer mà bạn đã quyết định: addon -> core, không ngược lại.
engine::KeyEvent toCoreKeyEvent(const Key &key) {
    engine::KeyEvent ev;

    if (key.isSimple()) {
        // phím in được (a-z, dấu câu, ...)
        ev.kind = engine::KeyEvent::Kind::Character;
        ev.character = static_cast<char32_t>(key.sym());
    } else if (key.check(FcitxKey_BackSpace)) {
        ev.kind = engine::KeyEvent::Kind::Backspace;
    } else if (key.check(FcitxKey_Delete)) {
        ev.kind = engine::KeyEvent::Kind::Delete;
    } else {
        ev.kind = engine::KeyEvent::Kind::Other;
    }

    return ev;
}

} // namespace

VnKeyEngine::VnKeyEngine(Instance *instance)
    : instance_(instance),
      factory_([](InputContext &) { return new engine::InputProcessor(); }) {
    instance_->inputContextManager().registerProperty("vnkeyProcessor", &factory_);
}

void VnKeyEngine::activate(const InputMethodEntry &, InputContextEvent &event) {
    // Reset buffer khi engine được kích hoạt cho 1 context mới,
    // tránh dính state cũ từ lần gõ trước.
    auto *ic = event.inputContext();
    auto *processor = ic->propertyFor(&factory_);
    processor->reset();
}

void VnKeyEngine::reset(const InputMethodEntry &, InputContextEvent &event) {
    auto *ic = event.inputContext();
    auto *processor = ic->propertyFor(&factory_);
    processor->reset();
    ic->inputPanel().reset();
    ic->updatePreedit();
}

void VnKeyEngine::keyEvent(const InputMethodEntry &, KeyEvent &keyEvent) {
    if (keyEvent.isRelease()) {
        return;
    }

    auto *ic = keyEvent.inputContext();
    auto *processor = ic->propertyFor(&factory_);

    engine::KeyEvent coreEvent = toCoreKeyEvent(keyEvent.key());

    // Nếu processor không quan tâm phím này (vd phím mũi tên, F1..),
    // để Fcitx5 forward cho ứng dụng xử lý bình thường.
    engine::ProcessResult result = processor->process(coreEvent);
    if (!result.consumed) {
        return;
    }

    keyEvent.filterAndAccept();

    // Cập nhật preedit (chữ đang gõ dở, chưa commit)
    Text preeditText;
    preeditText.append(result.preedit);
    preeditText.setCursor(result.preedit.size());
    ic->inputPanel().setPreedit(preeditText);
    ic->updatePreedit();

    // Nếu processor báo có ký tự cần commit ra ứng dụng (vd sau khi gõ
    // xong 1 âm tiết và gặp dấu cách/dấu câu), commit nó
    if (!result.commitText.empty()) {
        ic->commitString(result.commitText);
    }
}

FCITX_ADDON_FACTORY(VnKeyEngineFactory);