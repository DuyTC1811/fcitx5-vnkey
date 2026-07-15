#pragma once
#include <memory>
#include <optional>

#include "syllable.hpp"

namespace engine {
    // Mot phep bien doi ma phim gay ra tren am tiet hien tai
    struct Transform {
        enum class Kind {
            TONE,
            MARK,
            CANCEL_TONE,
            CANCEL_MARK,
            NONE
        };

        Kind kind = Kind::NONE;
        int value = 0;
    };

    inline constexpr int MARK_CIRCUMFLEX = 1; // â ê ô  (aa/ee/oo)
    inline constexpr int MARK_HORN = 2; // ă ơ ư  (aw/ow/uw)
    inline constexpr int MARK_DSTROKE = 3; // đ      (dd)

    class InputMethodDef {
    public:
        virtual ~InputMethodDef() = default;

        // key co gay bien doi tren am tiet hien tai khong?
        // nullopt = phim thuong, cu append vao buffer
        [[nodiscard]] virtual std::optional<Transform> match(const Syllable &current, char32_t key) const = 0;

        [[nodiscard]] virtual const char *name() const = 0;
    };

    std::unique_ptr<InputMethodDef> makeTelex();

    std::unique_ptr<InputMethodDef> makeVni();
} // END engine
