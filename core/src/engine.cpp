#include "engine.hpp"

#include <memory>
#include <ranges>
#include <string>

#include "chars.hpp"
#include "input_method.hpp"

namespace engine {
    std::string toUtf8(std::u32string_view const s) {
        std::string out;
        out.reserve(s.size() * 2);
        for (char32_t const c: s) {
            if (c < 0x80) {
                out += static_cast<char>(c);
            } else if (c < 0x800) {
                out += static_cast<char>(0xC0 | (c >> 6));
                out += static_cast<char>(0x80 | (c & 0x3F));
            } else if (c < 0x10000) {
                out += static_cast<char>(0xE0 | (c >> 12));
                out += static_cast<char>(0x80 | ((c >> 6) & 0x3F));
                out += static_cast<char>(0x80 | (c & 0x3F));
            } else {
                out += static_cast<char>(0xF0 | (c >> 18));
                out += static_cast<char>(0x80 | ((c >> 12) & 0x3F));
                out += static_cast<char>(0x80 | ((c >> 6) & 0x3F));
                out += static_cast<char>(0x80 | (c & 0x3F));
            }
        }
        return out;
    }

    // ---- Helpers dau chu: thuoc ve "ap dung Transform", nen nam o engine ----
    namespace {
        char32_t applyCircumflex(const char32_t c) {
            switch (c) {
                case U'a':
                    return U'â';
                case U'e':
                    return U'ê';
                case U'o':
                    return U'ô';
                default:
                    return c;
            }
        }

        char32_t applyHorn(const char32_t c) {
            switch (c) {
                case U'a':
                    return U'ă';
                case U'o':
                    return U'ơ';
                case U'u':
                    return U'ư';
                default:
                    return c;
            }
        }

        char32_t stripMark(const char32_t c) {
            switch (c) {
                case U'â':
                case U'ă':
                    return U'a';
                case U'ê':
                    return U'e';
                case U'ô':
                case U'ơ':
                    return U'o';
                case U'ư':
                    return U'u';
                default:
                    return c;
            }
        }

        // Ap dau chu vao vowel (luon lowercase)
        void applyMarkToVowel(std::u32string& vowel, const int mark, const std::u32string& initial) {
            if (mark == MARK_HORN && initial != U"qu" && initial != U"q") {
                for (size_t i = 0; i + 1 < vowel.size(); ++i) {
                    // Cap "uo" -> "ươ": horn ca doi (huowng -> hương)
                    if (vowel[i] == U'u' && vowel[i + 1] == U'o') {
                        vowel[i] = U'ư';
                        vowel[i + 1] = U'ơ';
                        return;
                    }
                    // Cap "ua" -> "ưa": horn vao u, KHONG phai a (ruawr -> rửa, muaw -> mưa)
                    // Voi q/qu thi u thuoc phu am dau, a moi nhan horn (quawng -> quăng)
                    if (vowel[i] == U'u' && vowel[i + 1] == U'a') {
                        vowel[i] = U'ư';
                        return;
                    }
                }
            }
            const auto apply = mark == MARK_CIRCUMFLEX ? applyCircumflex : applyHorn;
            for (char32_t& it: std::views::reverse(vowel)) {
                if (const char32_t m = apply(it); m != it) {
                    it = m;
                    break;
                }
            }
        }
    } // namespace

    InputProcessor::InputProcessor(std::unique_ptr<InputMethodDef> method, const Config cfg) :
        method_(std::move(method)), cfg_(cfg) {}

    InputProcessor::~InputProcessor() = default;

    void InputProcessor::reset() {
        raw_.clear();
        syl_ = Syllable{};
    }

    std::string InputProcessor::preedit() const {
        return toUtf8(syl_.compose(cfg_.newToneStyle));
    }

    Result InputProcessor::show() const {
        return {Action::UPDATE_PREEDIT, preedit()};
    }
    void InputProcessor::feedChar(const char32_t orig) {
        const char32_t low = toLowerVi(orig);
        const bool upper = low != orig;
        raw_ += orig; // giu phim goc cho ESCAPE va replay

        // syl_ luon lowercase -> match truc tiep, telex khong can biet ve case
        if (const auto tf = method_->match(syl_, low)) {
            switch (tf->kind) {
                case Transform::Kind::TONE:
                    syl_.tone = tf->value;
                    return;

                case Transform::Kind::CANCEL_TONE:
                    syl_.tone = 0;
                    syl_.pushCoda(low, upper); // "más" + s -> "mas"
                    return;

                case Transform::Kind::MARK:
                    if (tf->value == MARK_DSTROKE) {
                        syl_.initial = U"đ"; // cung do dai -> co case giu nguyen
                    } else {
                        applyMarkToVowel(syl_.vowel, tf->value, syl_.initial);
                    }
                    return;

                case Transform::Kind::CANCEL_MARK:
                    if (tf->value == MARK_DSTROKE) {
                        syl_.initial = U"d";
                        syl_.pushCoda(low, upper); // "đ" + d -> "dd"
                    } else {
                        for (auto& c: syl_.vowel) {
                            c = stripMark(c);
                        }
                        syl_.pushVowel(low, upper); // "â" + a -> "aa"
                    }
                    return;

                case Transform::Kind::NONE:
                    break;
            }
        }

        // ---- Phim thuong: phan loai theo lowercase ----
        switch (low) {
            case U'a':
            case U'e':
            case U'i':
            case U'o':
            case U'u':
            case U'y':
                if (syl_.coda.empty()) {
                    syl_.pushVowel(low, upper);
                } else {
                    syl_.pushCoda(low, upper);
                }
                break;
            default:
                if (syl_.vowel.empty()) {
                    syl_.pushInitial(low, upper);
                } else {
                    syl_.pushCoda(low, upper);
                }
                break;
        }
    }

    Result InputProcessor::process(const KeyInput key) {
        // Helper: chot chu dang go + van chuyen phim goc cho app
        const auto commitAndForward = [this] {
            Result r{Action::COMMIT, preedit()};
            r.forwardKey = true;
            reset();
            return r;
        };

        if (key.ctrlOrAlt) {
            if (raw_.empty()) {
                return {Action::PASS_THROUGH, ""};
            }
            return commitAndForward(); // Ctrl+S giua chung: chot chu, app van nhan Ctrl+S
        }

        using S = KeyInput::Special;
        switch (key.special) {
            case S::BACKSPACE: {
                if (raw_.empty()) {
                    return {Action::PASS_THROUGH, ""};
                }
                // Replay: bo phim cuoi, dung lai syl_ tu dau — raw_/syl_ khong bao gio lech
                const std::u32string keys(raw_.begin(), raw_.end() - 1);
                reset();
                for (const char32_t c: keys) {
                    feedChar(c);
                }
                return show();
            }

            case S::DELETE: {
                if (raw_.empty()) {
                    return {Action::PASS_THROUGH, ""};
                }
                if (syl_.tone == 0) {
                    return commitAndForward(); // khong co dau -> chot chu, app tu xoa ben phai
                }
                syl_.tone = 0;
                return show();
            }

            case S::SPACE: {
                if (raw_.empty()) {
                    return {Action::PASS_THROUGH, ""};
                }
                // Commit kem luon khoang trang: "hoà" + space -> "hoà "
                Result r{Action::COMMIT, preedit() + " "};
                reset();
                return r;
            }

            case S::ENTER:
            case S::TAB: {
                if (raw_.empty()) {
                    return {Action::PASS_THROUGH, ""};
                }
                // Commit chu, roi van chuyen phim goc cho app
                // (Enter co the la "gui tin nhan", Tab la nhay o — khong duoc nuot)
                return commitAndForward();
            }

            case S::ESCAPE: {
                // Huy che bien: commit lai dung chuoi phim tho da go ("mas" thay vi "má")
                if (raw_.empty()) {
                    return {Action::PASS_THROUGH, ""};
                }
                Result r{Action::COMMIT, toUtf8(raw_)};
                reset();
                return r;
            }

            case S::NONE:
                break;
        }

        if (key.ch == 0) { // mui ten, Home, F1... — chot chu truoc khi cursor chay di
            if (raw_.empty()) {
                return {Action::PASS_THROUGH, ""};
            }
            return commitAndForward();
        }

        feedChar(key.ch);
        return show();
    }
} // namespace engine
