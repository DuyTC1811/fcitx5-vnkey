#pragma once

#include <fcitx/addonfactory.h>
#include <fcitx/addonmanager.h>
#include <fcitx/inputcontextproperty.h>
#include <fcitx/inputmethodengine.h>
#include <fcitx/instance.h>

#include "engine.hpp" // core: engine::InputProcessor, engine::Result, makeTelex()

// State theo tung InputContext (moi o nhap lieu / cua so co buffer rieng).
// InputProcessor la lop core thuan, khong the ke thua lop cua Fcitx5,
// nen ta boc no vao mot InputContextProperty de Fcitx5 quan ly vong doi.
class VnKeyState final : public fcitx::InputContextProperty {
public:
    VnKeyState() : processor_(engine::makeTelex()) {}
    engine::InputProcessor processor_;
};

class VnKeyEngine final : public fcitx::InputMethodEngine {
public:
    explicit VnKeyEngine(fcitx::Instance *instance);

    void keyEvent(const fcitx::InputMethodEntry& entry, fcitx::KeyEvent& keyEvent) override;
    void reset(const fcitx::InputMethodEntry& entry, fcitx::InputContextEvent& event) override;
    void activate(const fcitx::InputMethodEntry& entry, fcitx::InputContextEvent& event) override;

private:
    fcitx::Instance *instance_;
    fcitx::FactoryFor<VnKeyState> factory_;
};

class VnKeyEngineFactory final : public fcitx::AddonFactory {
public:
    fcitx::AddonInstance *create(fcitx::AddonManager *manager) override {
        return new VnKeyEngine(manager->instance());
    }
};
