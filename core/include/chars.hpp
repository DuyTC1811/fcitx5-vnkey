#pragma once

namespace engine {
    // Case mapping bang chu cai tieng Viet (khong dung towlower vi phu thuoc locale)
    [[nodiscard]] constexpr char32_t toLowerVi(const char32_t c) {
        if (c >= U'A' && c <= U'Z') {
            return c + 0x20;
        }
        if (c >= 0xC0 && c <= 0xDE && c != 0xD7) { // À..Ý dai Latin-1
            return c + 0x20;
        }
        switch (c) {
            case U'Ă':
                return U'ă';
            case U'Đ':
                return U'đ';
            case U'Ĩ':
                return U'ĩ';
            case U'Ũ':
                return U'ũ';
            case U'Ơ':
                return U'ơ';
            case U'Ư':
                return U'ư';
            default:
                break;
        }
        if (c >= 0x1EA0 && c <= 0x1EF9) { // Ạ..ỹ: codepoint chan = hoa
            return (c % 2 == 0) ? c + 1 : c;
        }
        return c;
    }

    [[nodiscard]] constexpr char32_t toUpperVi(const char32_t c) {
        if (c >= U'a' && c <= U'z') {
            return c - 0x20;
        }
        if (c >= 0xE0 && c <= 0xFE && c != 0xF7) {
            return c - 0x20;
        }
        switch (c) {
            case U'ă':
                return U'Ă';
            case U'đ':
                return U'Đ';
            case U'ĩ':
                return U'Ĩ';
            case U'ũ':
                return U'Ũ';
            case U'ơ':
                return U'Ơ';
            case U'ư':
                return U'Ư';
            default:
                break;
        }
        if (c >= 0x1EA1 && c <= 0x1EF9 && c % 2 == 1) {
            return c - 1;
        }
        return c;
    }
} // namespace engine
