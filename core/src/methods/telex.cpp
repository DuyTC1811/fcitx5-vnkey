#include <ranges>

#include "input_method.hpp"

namespace engine {
    namespace {
        bool contains(const std::u32string &s, const char32_t c) {
            return s.find(c) != std::u32string::npos;
        }

        bool endsWith(const std::u32string &s, char32_t c) {
            return !s.empty() && s.back() == c;
        }

        class Telex final : public InputMethodDef {
        public:
            [[nodiscard]] const char *name() const override {
                return "telex";
            }

            [[nodiscard]] std::optional<Transform>
            match(const Syllable &cur, const char32_t key) const override {

                // dd -> đ: phai check TRUOC valid(), vi buffer "d" chua co nguyen am
                if (key == U'd') {
                    if (cur.initial == U"đ")
                        return Transform{
                            Transform::Kind::CANCEL_MARK, MARK_DSTROKE
                        };
                    if (cur.initial == U"d" && cur.vowel.empty())
                        return Transform{
                            Transform::Kind::MARK, MARK_DSTROKE
                        };
                    // roi xuong duoi: 'd' sau nguyen am la phu am cuoi binh thuong
                }

                if (!cur.valid()) return std::nullopt;

                // 1. THANH ĐIỆU: s f r x j
                if (const int tone = toneOf(key); tone != 0) {
                    if (cur.tone == tone) // go lap -> huy
                        return Transform{Transform::Kind::CANCEL_TONE, tone};
                    return Transform{Transform::Kind::TONE, tone};
                }

                // 2. DẤU CHỮ
                switch (key) {
                    case U'a': // aa -> â, aaa -> huy
                        if (contains(cur.vowel, U'â'))
                            return Transform{Transform::Kind::CANCEL_MARK, MARK_CIRCUMFLEX};
                        if (endsWith(cur.vowel, U'a'))
                            return Transform{Transform::Kind::MARK, MARK_CIRCUMFLEX};
                        break;

                    case U'e': // ee -> ê
                        if (contains(cur.vowel, U'ê'))
                            return Transform{
                                Transform::Kind::CANCEL_MARK, MARK_CIRCUMFLEX
                            };
                        if (endsWith(cur.vowel, U'e'))
                            return Transform{
                                Transform::Kind::MARK, MARK_CIRCUMFLEX
                            };
                        break;

                    case U'o': // oo -> ô
                        if (contains(cur.vowel, U'ô'))
                            return Transform{
                                Transform::Kind::CANCEL_MARK, MARK_CIRCUMFLEX
                            };
                        if (endsWith(cur.vowel, U'o'))
                            return Transform{
                                Transform::Kind::MARK, MARK_CIRCUMFLEX
                            };
                        break;

                    case U'w': // aw -> ă, ow -> ơ, uw -> ư
                        for (const char32_t it : std::views::reverse(cur.vowel))
                            if (it == U'a' || it == U'o' || it == U'u')
                                return Transform{
                                    Transform::Kind::MARK, MARK_HORN
                                };

                        // Het ung vien ma da co dau horn -> go lap la huy
                        if (contains(cur.vowel, U'ă') || contains(cur.vowel, U'ơ')
                            || contains(cur.vowel, U'ư'))
                            return Transform{Transform::Kind::CANCEL_MARK, MARK_HORN};
                        break;

                    case U'd': // dd -> đ (dau chu tren PHU AM DAU, khong phai vowel)
                        if (cur.initial == U"đ")
                            return Transform{Transform::Kind::CANCEL_MARK, MARK_DSTROKE};
                        if (cur.initial == U"d")
                            return Transform{Transform::Kind::MARK, MARK_DSTROKE};
                        break;

                    default:
                        break;
                }
                return std::nullopt; // phim thuong -> append
            }

        private:
            static int toneOf(const char32_t key) {
                switch (key) {
                    case U's': return 1; // '
                    case U'f': return 2; // `
                    case U'r': return 3; // ?
                    case U'x': return 4; // ~
                    case U'j': return 5; // .
                    default: return 0;
                }
            }
        };
    } // namespace

    std::unique_ptr<InputMethodDef> makeTelex() {
        return std::make_unique<Telex>();
    }
} // namespace engine
