#include "test_helpers.h"
#include "../../core/include/engine.hpp"
#include "../../core/include/input_method.hpp"

void test_engine_basics() {
    using namespace engine;

    std::printf("\n--- ENGINE BASICS ---\n");

    // TEST 1: Khởi tạo engine
    {
        InputProcessor p(makeTelex());
        CHECK_EQ(p.preedit(), "", "Init: buffer rỗng");
    }

    // TEST 2: Append ký tự thường
    {
        InputProcessor p(makeTelex());

        KeyInput k;
        k.ch = U'a';
        auto r = p.process(k);

        CHECK_EQ_INT(r.action, Action::UPDATE_PREEDIT, "Append 'a': action = UPDATE_PREEDIT");
        CHECK_EQ(r.text, "a", "Append 'a': text = 'a'");
        CHECK_EQ(p.preedit(), "a", "Append 'a': buffer = 'a'");
    }

    // TEST 3: Append nhiều ký tự
    {
        InputProcessor p(makeTelex());

        for (char c: {'t', 'o', 'a', 'n'}) {
            KeyInput k;
            k.ch = static_cast<char32_t>(c);
            p.process(k);
        }

        CHECK_EQ(p.preedit(), "toan", "Append 'toan': buffer = 'toan'");
    }

    // TEST 4: Backspace — xóa ký tự cuối

    {
        InputProcessor p(makeTelex());

        // Gõ "toan"
        for (char c: {'t', 'o', 'a', 'n'}) {
            KeyInput k;
            k.ch = static_cast<char32_t>(c);
            p.process(k);
        }

        // Backspace 1 lần
        KeyInput bs;
        bs.special = KeyInput::Special::BACKSPACE;
        auto r = p.process(bs);

        CHECK_EQ_INT(r.action, Action::UPDATE_PREEDIT, "Backspace: action = UPDATE_PREEDIT");
        CHECK_EQ(r.text, "toa", "Backspace: text = 'toa'");
        CHECK_EQ(p.preedit(), "toa", "Backspace: buffer = 'toa'");
    }

    // TEST 5: Backspace trên buffer rỗng — passthrough
    {
        InputProcessor p(makeTelex());

        KeyInput bs;
        bs.special = KeyInput::Special::BACKSPACE;
        auto r = p.process(bs);

        CHECK_EQ_INT(r.action, Action::PASS_THROUGH, "Backspace trên buffer rỗng: PASS_THROUGH");
        CHECK_EQ(p.preedit(), "", "Backspace trên buffer rỗng: buffer vẫn rỗng");
    }
    // TEST 6: Space — commit buffer

    // ---- 1. SPACE: commit kem khoang trang ----
    {
        InputProcessor p(makeTelex());
        for (char c: {'a', 'b', 'c'}) {
            KeyInput k;
            k.ch = static_cast<char32_t>(c);
            p.process(k);
        }
        KeyInput sp;
        sp.special = KeyInput::Special::SPACE;
        auto r = p.process(sp);
        CHECK_EQ_INT(r.action, Action::COMMIT, "Space: action = COMMIT");
        CHECK_EQ(r.text, "abc ", "Space: commit text = 'abc ' (kem khoang trang)");
        CHECK_EQ_INT(r.forwardKey, false, "Space: khong forward phim (space da nam trong text)");
        CHECK_EQ(p.preedit(), "", "Space: buffer reset ve rong");
    }

    // TEST 7: Space trên buffer rỗng — passthrough

    {
        InputProcessor p(makeTelex());

        KeyInput sp;
        sp.special = KeyInput::Special::SPACE;
        auto r = p.process(sp);

        CHECK_EQ_INT(r.action, Action::PASS_THROUGH, "Space trên buffer rỗng: PASS_THROUGH");
    }

    // TEST 8: Enter — commit buffer
    {
        InputProcessor p(makeTelex());

        for (char c: {'x', 'y', 'z'}) {
            KeyInput k;
            k.ch = static_cast<char32_t>(c);
            p.process(k);
        }

        KeyInput enter;
        enter.special = KeyInput::Special::ENTER;
        auto r = p.process(enter);

        CHECK_EQ_INT(r.action, Action::COMMIT, "Enter: action = COMMIT");
        CHECK_EQ(r.text, "xyz", "Enter: commit text = 'xyz'");
        CHECK_EQ(p.preedit(), "", "Enter: buffer reset");
    }
    // TEST 11: reset() — làm sạch buffer
    {
        InputProcessor p(makeTelex());

        for (char c: {'h', 'e', 'l', 'l', 'o'}) {
            KeyInput k;
            k.ch = static_cast<char32_t>(c);
            p.process(k);
        }

        CHECK_EQ(p.preedit(), "hello", "Before reset: buffer = 'hello'");
        p.reset();
        CHECK_EQ(p.preedit(), "", "After reset: buffer rỗng");
    }

    // TEST 12: Multiple sessions — reset giữa các từ
    {
        InputProcessor p(makeTelex());

        // Từ 1
        for (char c: {'a', 'b'}) {
            KeyInput k;
            k.ch = static_cast<char32_t>(c);
            p.process(k);
        }
        KeyInput sp;
        sp.special = KeyInput::Special::SPACE;
        p.process(sp);

        // Từ 2
        for (char c: {'c', 'd'}) {
            KeyInput k;
            k.ch = static_cast<char32_t>(c);
            p.process(k);
        }

        CHECK_EQ(p.preedit(), "cd", "Multiple sessions: buffer = 'cd'");
    }

    // TEST 13: Preedit chính xác
    {
        InputProcessor p(makeTelex());

        KeyInput k;
        k.ch = U'x';
        auto r1 = p.process(k);
        CHECK_EQ(r1.text, "x", "Preedit sau 'x': text = 'x'");

        k.ch = U'y';
        auto r2 = p.process(k);
        CHECK_EQ(r2.text, "xy", "Preedit sau 'xy': text = 'xy'");

        k.ch = U'z';
        auto r3 = p.process(k);
        CHECK_EQ(r3.text, "xyz", "Preedit sau 'xyz': text = 'xyz'");
    }


    // TEST 14: Tab — commit

    {
        InputProcessor p(makeTelex());

        for (char c: {'t', 'a', 'b'}) {
            KeyInput k;
            k.ch = static_cast<char32_t>(c);
            p.process(k);
        }

        KeyInput tab;
        tab.special = KeyInput::Special::TAB;
        auto r = p.process(tab);

        CHECK_EQ_INT(r.action, Action::COMMIT, "Tab: action = COMMIT");
        CHECK_EQ(r.text, "tab", "Tab: commit text = 'tab'");
    }

    // TEST 15: Backspace nhiều lần

    {
        InputProcessor p(makeTelex());

        for (char c: {'a', 'b', 'c', 'd'}) {
            KeyInput k;
            k.ch = static_cast<char32_t>(c);
            p.process(k);
        }

        // Backspace 2 lần
        KeyInput bs;
        bs.special = KeyInput::Special::BACKSPACE;
        p.process(bs);
        p.process(bs);

        CHECK_EQ(p.preedit(), "ab", "Backspace 2 lần: buffer = 'ab'");
    }

}

