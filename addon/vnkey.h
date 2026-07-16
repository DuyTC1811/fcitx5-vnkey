#ifndef _FCITX5_VNKEY_VNKEY_H_
#define _FCITX5_VNKEY_VNKEY_H_

// addon/vnkey.h
// Thin Fcitx5 wrapper around vietcore::InputProcessor.
// This is the ONLY layer that touches Fcitx5 headers — core stays clean.

#include <fcitx/addonfactory.h>
#include <fcitx/addoninstance.h>
#include <fcitx/inputcontextproperty.h>
#include <fcitx/inputmethodengine.h>
#include <fcitx/instance.h>

#include <memory>

#include "core/input_processor.hpp"  // <-- adjust path to your real header location

namespace fcitx {

    // Per-input-context state. Every text field gets its own InputProcessor so
    // buffers never bleed between windows/apps.
    class VnKeyState : public InputContextProperty
    {
    public:
        explicit VnKeyState(InputContext* ic) : ic_(ic) {}

        vietcore::InputProcessor& processor() { return processor_; }
        InputContext* ic() { return ic_; }

    private:
        InputContext* ic_;
        // TODO: match your real InputProcessor constructor. If it needs a config
        // object (e.g. makeTelex()), pass it here.
        vietcore::InputProcessor processor_{vietcore::makeTelex()};
    };

    class VnKeyEngine : public InputMethodEngineV3
    {
    public:
        explicit VnKeyEngine(Instance* instance);

        void keyEvent(const InputMethodEntry& entry, KeyEvent& keyEvent) override;
        void reset(const InputMethodEntry& entry, InputContextEvent& event) override;

        Instance* instance() { return instance_; }

    private:
        void updatePreedit(InputContext* ic, VnKeyState* state);
        void commitBuffer(InputContext* ic, VnKeyState* state);

        Instance* instance_;
        FactoryFor<VnKeyState> factory_;
    };

    class VnKeyEngineFactory : public AddonFactory
    {
    public:
        AddonInstance* create(AddonManager* manager) override
        {
            return new VnKeyEngine(manager->instance());
        }
    };

}  // namespace fcitx

#endif  // _FCITX5_VNKEY_VNKEY_H_