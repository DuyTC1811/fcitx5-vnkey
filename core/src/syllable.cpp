#include "syllable.hpp"

#include <cassert>

#include "chars.hpp"

namespace engine {
    namespace {
        // Bang NFC: nguyen am goc x thanh dieu -> ky tu precomposed (1 codepoint)
        constexpr std::u32string_view BASES = U"aăâeêioôơuưy";
        constexpr char32_t TONE_TABLE[12][6] = {
                {U'a', U'á', U'à', U'ả', U'ã', U'ạ'}, {U'ă', U'ắ', U'ằ', U'ẳ', U'ẵ', U'ặ'},
                {U'â', U'ấ', U'ầ', U'ẩ', U'ẫ', U'ậ'}, {U'e', U'é', U'è', U'ẻ', U'ẽ', U'ẹ'},
                {U'ê', U'ế', U'ề', U'ể', U'ễ', U'ệ'}, {U'i', U'í', U'ì', U'ỉ', U'ĩ', U'ị'},
                {U'o', U'ó', U'ò', U'ỏ', U'õ', U'ọ'}, {U'ô', U'ố', U'ồ', U'ổ', U'ỗ', U'ộ'},
                {U'ơ', U'ớ', U'ờ', U'ở', U'ỡ', U'ợ'}, {U'u', U'ú', U'ù', U'ủ', U'ũ', U'ụ'},
                {U'ư', U'ứ', U'ừ', U'ử', U'ữ', U'ự'}, {U'y', U'ý', U'ỳ', U'ỷ', U'ỹ', U'ỵ'},
        };

        char32_t withTone(const char32_t base, const int tone) {
            assert(tone >= 1 && tone <= 5);
            const auto pos = BASES.find(base);
            if (pos == std::u32string_view::npos) {
                return base;
            }
            return TONE_TABLE[pos][tone];
        }

        bool isMarkedVowel(const char32_t c) {
            return c == U'ă' || c == U'â' || c == U'ê' || c == U'ô' || c == U'ơ' || c == U'ư';
        }
    } // namespace

    void Syllable::pushInitial(const char32_t c, const bool upper) {
        initial += c;
        upInitial.push_back(upper);
    }

    void Syllable::pushVowel(const char32_t c, const bool upper) {
        vowel += c;
        upVowel.push_back(upper);
    }

    void Syllable::pushCoda(const char32_t c, const bool upper) {
        coda += c;
        upCoda.push_back(upper);
    }

    void Syllable::popBack() {
        if (!coda.empty()) {
            coda.pop_back();
            upCoda.pop_back();
        } else if (!vowel.empty()) {
            vowel.pop_back();
            upVowel.pop_back();
        } else if (!initial.empty()) {
            initial.pop_back();
            upInitial.pop_back();
        }
        if (vowel.empty()) {
            tone = 0; // het nguyen am -> mat thanh
        }
    }

    bool Syllable::valid() const {
        return !vowel.empty();
    }

    std::u32string Syllable::compose(const bool newToneStyle) const {
        std::u32string v = vowel;
        if (tone >= 1 && tone <= 5 && !v.empty()) {
            if (const int pos = tonePosition(newToneStyle); pos >= 0) {
                v[pos] = withTone(v[pos], tone);
            }
        }

        std::u32string out = initial + v + coda;
        // Ap case dung MOT cho duy nhat. Moi transform deu bao toan do dai
        // (d->đ, uo->ươ, a->á) nen cac co luon thang hang voi out.
        size_t i = 0;
        const auto applyFlags = [&](const std::vector<bool>& flags) {
            for (const bool up: flags) {
                if (up) {
                    out[i] = toUpperVi(out[i]);
                }
                ++i;
            }
        };
        applyFlags(upInitial);
        applyFlags(upVowel);
        applyFlags(upCoda);
        return out;
    }

    int Syllable::tonePosition(const bool newToneStyle) const {
        if (vowel.empty()) {
            return -1;
        }

        // Quy tac 1: co nguyen am mang dau chu (â ê ô ơ ư ă) -> dau thanh dat vao no
        // Lay cai CUOI (uong -> dau tren ơ: "ươ")
        for (int i = static_cast<int>(vowel.size()) - 1; i >= 0; --i) {
            if (isMarkedVowel(vowel[i])) {
                return i;
            }
        }

        // Quy tac 2: co phu am cuoi -> dau tren nguyen am cuoi ("toán", "học")
        if (!coda.empty()) {
            return static_cast<int>(vowel.size()) - 1;
        }

        // Quy tac 3: khong phu am cuoi
        if (vowel.size() == 1) {
            return 0; // "má"
        }
        if (vowel.size() == 3) {
            return 1; // "ngoại" -> giua
        }
        // 2 nguyen am: oa/oe/uy kieu moi -> dau cuoi ("hoá"); kieu cu -> dau dau ("hóa")
        if (vowel == U"oa" || vowel == U"oe" || vowel == U"uy") {
            return newToneStyle ? 1 : 0;
        }
        return 0; // "của", "mía" -> dau dau
    }

    bool Syllable::broken() const {
        return coda.find_first_of(U"aeiouy") != std::u32string::npos;
    }

} // namespace engine
