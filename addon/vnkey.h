#ifndef _FCITX5_VNKEY_VNKEY_H_
#define _FCITX5_VNKEY_VNKEY_H_

// addon/vnkey.h
// Lớp Fcitx5 mỏng bọc engine::InputProcessor. Đây là lớp DUY NHẤT chạm tới
// header Fcitx5 — core giữ nguyên độc lập.

#include <fcitx/addonfactory.h>
#include <fcitx/addoninstance.h>
#include <fcitx/inputcontextproperty.h>
#include <fcitx/inputmethodengine.h>
#include <fcitx/instance.h>

#include "engine.hpp"        // engine::InputProcessor, Result, Action, Config
#include "input_method.hpp"  // engine::makeTelex()

namespace fcitx {
    // Trạng thái theo từng input context. Mỗi ô nhập liệu có InputProcessor riêng
    // để buffer không lẫn giữa các cửa sổ/ứng dụng.
    class VnKeyState : public InputContextProperty {
    public:
        explicit VnKeyState(InputContext *ic)
            : ic_(ic), processor_(engine::makeTelex()) {
        }

        engine::InputProcessor &processor() { return processor_; }
        InputContext *ic() { return ic_; }

    private:
        InputContext *ic_;
        engine::InputProcessor processor_;
    };

    class VnKeyEngine : public InputMethodEngineV3 {
    public:
        explicit VnKeyEngine(Instance *instance);

        void keyEvent(const InputMethodEntry &entry, KeyEvent &keyEvent) override;

        void reset(const InputMethodEntry &entry, InputContextEvent &event) override;

        Instance *instance() { return instance_; }

    private:
        void showPreedit(InputContext *ic, const std::string &text);

        void clearPreedit(InputContext *ic);

        Instance *instance_;
        FactoryFor<VnKeyState> factory_;
    };

    class VnKeyEngineFactory : public AddonFactory {
    public:
        AddonInstance *create(AddonManager *manager) override {
            return new VnKeyEngine(manager->instance());
        }
    };
} // namespace fcitx

#endif  // _FCITX5_VNKEY_VNKEY_H_
