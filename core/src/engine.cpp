#include "engine.hpp"

#include <memory>
#include <ranges>
#include <string>

#include "input_method.hpp"

namespace engine {
    std::string toUtf8(std::u32string_view const s) {
        std::string out;
        out.reserve(s.size() * 2);
        for (char32_t const c: s) {
            if (c < 0x80) out += static_cast<char>(c);
            else if (c < 0x800) {
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
                case U'a': return U'â';
                case U'e': return U'ê';
                case U'o': return U'ô';
                default: return c;
            }
        }

        char32_t applyHorn(const char32_t c) {
            switch (c) {
                case U'a': return U'ă';
                case U'o': return U'ơ';
                case U'u': return U'ư';
                default: return c;
            }
        }

        char32_t stripMark(const char32_t c) {
            switch (c) {
                case U'â':
                case U'ă': return U'a';
                case U'ê': return U'e';
                case U'ô':
                case U'ơ': return U'o';
                case U'ư': return U'u';
                default: return c;
            }
        }
    }

    InputProcessor::InputProcessor(std::unique_ptr<InputMethodDef> method,
                                   const Config cfg)
        : method_(std::move(method)), cfg_(cfg) {
    }

    InputProcessor::~InputProcessor() = default;

    void InputProcessor::reset() {
        raw_.clear();
        syl_ = Syllable{};
    }

    std::string InputProcessor::preedit() const {
        return toUtf8(syl_.compose(cfg_.newToneStyle));
    }

    Result InputProcessor::show() const {
        return {
            Action::UPDATE_PREEDIT, preedit()
        };
    }

    Result InputProcessor::process(const KeyInput key) {
        if (key.ctrlOrAlt) {
            reset();
            return {Action::PASS_THROUGH, ""};
        }

        using S = KeyInput::Special;
        switch (key.special) {
            case S::BACKSPACE: {
                if (raw_.empty()) return {Action::PASS_THROUGH, ""};
                // Xoa tren CAU TRUC am tiet, khong xoa tren chuoi compose
                raw_.pop_back();
                if (!syl_.coda.empty()) syl_.coda.pop_back();
                else if (!syl_.vowel.empty()) syl_.vowel.pop_back();
                else if (!syl_.initial.empty()) syl_.initial.pop_back();
                if (syl_.vowel.empty()) syl_.tone = 0; // het nguyen am -> mat thanh
                return show();
            }

            case S::DELETE: {
                if (raw_.empty() || syl_.tone == 0)
                    return {
                        Action::PASS_THROUGH, "" // khong co dau -> tra phim cho app
                    };
                syl_.tone = 0;
                return show();
            }

            case S::SPACE: {
                if (raw_.empty()) {
                    return {
                        Action::PASS_THROUGH, ""
                    };
                }
                // Commit kem luon khoang trang: "hoà" + space -> "hoà "
                Result r{
                    Action::COMMIT, preedit() + " "
                };
                reset();
                return r;
            }
            case S::ENTER:
            case S::TAB: {
                if (raw_.empty()) {
                    return {
                        Action::PASS_THROUGH, ""
                    };
                }
                // Commit chu, roi van chuyen phim goc cho app
                // (Enter co the la "gui tin nhan", Tab la nhay o — khong duoc nuot)
                Result r{
                    Action::COMMIT, preedit()
                };
                r.forwardKey = true;
                reset();
                return r;
            }

            case S::ESCAPE: {
                // Huy che bien: commit lai dung chuoi phim tho da go ("mas" thay vi "má")
                if (raw_.empty()) return {
                    Action::PASS_THROUGH, ""
                };
                Result r{
                    Action::COMMIT, toUtf8(raw_)
                };
                reset();
                return r;
            }

            case S::NONE:
                break;
        }

        if (key.ch == 0) return {
            Action::PASS_THROUGH, ""
        };

        // ---- Hoi kieu go: phim nay co phai lenh bien doi khong ----
        if (const auto tf = method_->match(syl_, key.ch)) {
            switch (tf->kind) {
                case Transform::Kind::TONE:
                    raw_ += key.ch;
                    syl_.tone = tf->value;
                    return show();

                case Transform::Kind::CANCEL_TONE:
                    // Go lap phim thanh: huy dau + tra ky tu goc ve cuoi
                    raw_ += key.ch;
                    syl_.tone = 0;
                    syl_.coda += key.ch;
                    return show();

                case Transform::Kind::MARK:
                    raw_ += key.ch;
                    if (tf->value == MARK_DSTROKE) {
                        syl_.initial = U"đ";
                    } else {
                        const auto apply = (tf->value == MARK_CIRCUMFLEX)
                                         ? applyCircumflex
                                         : applyHorn;
                        // Ap dung vao nguyen am phu hop CUOI CUNG (dung cho "uo" -> "uơ")
                        for (auto it = syl_.vowel.rbegin(); it != syl_.vowel.rend(); ++it)
                            if (const char32_t m = apply(*it); m != *it) {
                                *it = m;
                                break;
                            }
                    }
                    return show();

                case Transform::Kind::CANCEL_MARK:
                    raw_ += key.ch;
                    if (tf->value == MARK_DSTROKE) {
                        syl_.initial = U"d";
                        syl_.coda += key.ch; // "đ" + d -> "dd"
                    } else {
                        for (auto &c: syl_.vowel) c = stripMark(c);
                        syl_.vowel += key.ch; // "â" + a -> "aa"
                    }
                    return show();

                case Transform::Kind::NONE:
                    break;
            }
        }

        // ---- Phim thuong: append vao CAU TRUC am tiet ----
        raw_ += key.ch;
        // Nguyen am hay phu am? -> quyet dinh vao vowel/initial/coda
        switch (key.ch) {
            case U'a':
            case U'e':
            case U'i':
            case U'o':
            case U'u':
            case U'y':
                if (syl_.coda.empty()) syl_.vowel += key.ch;
                else syl_.coda += key.ch; // sau phu am cuoi -> chuoi la
                break;
            default:
                if (syl_.vowel.empty()) syl_.initial += key.ch;
                else syl_.coda += key.ch;
                break;
        }
        return show();
    }
}
