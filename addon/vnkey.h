#pragma once

#include <fcitx/inputmethodengine.h>
#include <fcitx/instance.h>
#include <fcitx/addonfactory.h>
#include <fcitx/addonmanager.h>

#include "engine/input_processor.h"  // lớp core của bạn, KHÔNG phụ thuộc Fcitx5

class VnKeyEngine final : public fcitx::InputMethodEngine {
public:
    explicit VnKeyEngine(fcitx::Instance *instance);

    // Được gọi mỗi khi user gõ 1 phím trong khi engine này đang active
    void keyEvent(const fcitx::InputMethodEntry &entry,
                  fcitx::KeyEvent &keyEvent) override;

    // Được gọi khi chuyển sang input method khác / mất focus / v.v.
    void reset(const fcitx::InputMethodEntry &entry,
               fcitx::InputContextEvent &event) override;

    // Được gọi khi user click chọn engine này lần đầu / activate lại context
    void activate(const fcitx::InputMethodEntry &entry,
                  fcitx::InputContextEvent &event) override;

private:
    fcitx::Instance *instance_;

    // Mỗi InputContext (mỗi cửa sổ/ô nhập liệu) cần 1 processor riêng,
    // vì trạng thái buffer_ là per-context, không phải global.
    // Dùng property để Fcitx5 tự quản lý vòng đời theo InputContext.
    fcitx::FactoryFor<engine::InputProcessor> factory_;
};

class VnKeyEngineFactory final : public fcitx::AddonFactory {
public:
    fcitx::AddonInstance *create(fcitx::AddonManager *manager) override {
        return new VnKeyEngine(manager->instance());
    }
};