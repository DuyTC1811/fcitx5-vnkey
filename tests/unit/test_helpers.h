#pragma once
#include <cstdio>
#include <string>

#include "engine.hpp"
inline int g_failures = 0;
inline int g_passes = 0;
using namespace engine;

// MACRO ASSERT
#define CHECK_EQ(actual, expected, desc)                                    \
    do {                                                                    \
        auto a = (actual);                                                  \
        auto e = (expected);                                                \
        if (a != e) {                                                       \
            ++g_failures;                                                   \
            std::printf("\n");                                              \
            std::printf("  ❌ FAIL: %s\n", desc);                            \
            std::printf("     File: %s:%d\n", __FILE__, __LINE__);          \
            std::printf("     Expected: \"%s\"\n",                          \
                        std::string(e).c_str());                            \
            std::printf("     Actual:   \"%s\"\n",                          \
                        std::string(a).c_str());                            \
            std::printf("\n");                                              \
        } else {                                                            \
            ++g_passes;                                                     \
            std::printf("  ✓ %s\n", desc);                                  \
        }                                                                   \
    } while (0)

// Variant cho enum/int — so sánh số
#define CHECK_EQ_INT(actual, expected, desc)                                \
    do {                                                                    \
        auto a = static_cast<int>(actual);                                  \
        auto e = static_cast<int>(expected);                                \
        if (a != e) {                                                       \
            ++g_failures;                                                   \
            std::printf("\n");                                              \
            std::printf("  ❌ FAIL: %s\n", desc);                            \
            std::printf("     File: %s:%d\n", __FILE__, __LINE__);          \
            std::printf("     Expected: %d\n", e);                          \
            std::printf("     Actual:   %d\n", a);                          \
            std::printf("\n");                                              \
        } else {                                                            \
            ++g_passes;                                                     \
            std::printf("  ✓ %s\n", desc);                                  \
        }                                                                   \
    } while (0)

// In tổng kết
inline void printTestSummary() {
    std::printf("\n");
    std::printf("══════════════════════════════════\n");
    std::printf("  Passed: %d ✓ | ", g_passes);
    if (g_failures > 0) {
        std::printf("Failed: %d ❌\n", g_failures);
    }
    std::printf("\n");
    std::printf("══════════════════════════════════\n");
}

// ============================================================================
// FEED — go mot chuoi phim ASCII, tra ve preedit hien tai
// ============================================================================
//
// Dung preedit() thay vi r.text vi:
//   - PASS_THROUGH tra text rong -> khong duoc coi la ket qua
//   - COMMIT xoa buffer -> preedit() = "" (dung, xem feedCommit ben duoi)
//   - preedit() LUON la trang thai that cua buffer
//
inline std::string feed(InputProcessor &p, const std::string_view keys) {
    for (const char c: keys) {
        KeyInput k;
        k.ch = static_cast<char32_t>(c);
        p.process(k);
    }
    return p.preedit();
}

// ----------------------------------------------------------------------------
// FEED + COMMIT — go chuoi phim roi nhan Space, tra ve chuoi da commit
// Dung cho test "nguoi dung go xong mot tu"
// ----------------------------------------------------------------------------
inline std::string feedCommit(InputProcessor &p, const std::string_view keys) {
    for (const char c: keys) {
        KeyInput k;
        k.ch = static_cast<char32_t>(c);
        p.process(k);
    }
    KeyInput sp;
    sp.special = KeyInput::Special::SPACE;
    return p.process(sp).text;
}

// ----------------------------------------------------------------------------
// FEED 1 PHIM — tra ve Result day du, dung khi can kiem tra action
// ----------------------------------------------------------------------------
inline Result feedKey(InputProcessor &p, char c) {
    engine::KeyInput k;
    k.ch = static_cast<char32_t>(c);
    return p.process(k);
}

// ----------------------------------------------------------------------------
// FEED PHIM DAC BIET — Backspace / Delete / Escape / Space / Enter / Tab
// ----------------------------------------------------------------------------
inline Result feedSpecial(InputProcessor &p, const KeyInput::Special sp) {
    KeyInput k;
    k.special = sp;
    return p.process(k);
}

// ----------------------------------------------------------------------------
// FEED SENTENCE — go ca cau, gap ' ' thi gia lap phim SPACE that
// Tra ve: chuoi da commit (kem dau cach) + phan preedit con dang do
// ----------------------------------------------------------------------------
inline std::string feedSentence(InputProcessor& p, const std::string_view keys) {
    std::string out;
    for (const char c : keys) {
        if (c == ' ') {
            KeyInput sp;
            sp.special = KeyInput::Special::SPACE;
            const auto r = p.process(sp);
            if (r.action == Action::COMMIT)
                out += r.text;              // tu vua duoc chot
            out += ' ';                     // dau cach do addon forward (xem ghi chu)
        } else {
            KeyInput k;
            k.ch = static_cast<char32_t>(c);
            p.process(k);
        }
    }
    out += p.preedit();                     // tu cuoi chua nhan Space
    return out;
}
