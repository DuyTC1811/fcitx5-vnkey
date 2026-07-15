#pragma once
#include <string>

#include "input_method.hpp"
#include "keyinput.hpp"

namespace engine {
    enum class Action {
        PASS_THROUGH, // core khong xu ly, tra phim lai cho app
        UPDATE_PREEDIT, // cap nhat preedit = text
        COMMIT, // commit text, xoa buffer
        COMMIT_WITH_BACKSPACE // gui `backspaces` phim xoa roi commit text (fake-backspace)
    };

    struct Result {
        Action action = Action::PASS_THROUGH;
        std::string text; // UTF-8
        int backspaces = 0;
    };

    struct Config {
        bool newToneStyle = true; // "hoá" (true) vs "hóa" (false)
        bool spellCheck = true; // chi bien doi khi am tiet hop le
    };

    // API DUY NHAT ma addon duoc phep dung.
    class InputProcessor {
    public:
        explicit InputProcessor(std::unique_ptr<InputMethodDef> method, Config cfg = {});

        ~InputProcessor();

        Result process(KeyInput key);

        void reset();

        [[nodiscard]] std::string preedit() const; // buffer hien tai duoi dang UTF-8

    private:
        Result show() const; // helper: compose + tra UPDATE_PREEDIT
        std::unique_ptr<InputMethodDef> method_;
        Config cfg_;
        std::u32string raw_; // phim tho nguoi dung da go: "mas"
        Syllable syl_; // trang thai am tiet: vowel="a", tone=1
    };

    // Tien ich chuyen doi (implement trong engine.cpp)
    std::string toUtf8(std::u32string_view s);
} // END engine
