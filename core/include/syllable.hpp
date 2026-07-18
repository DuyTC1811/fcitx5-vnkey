#pragma once
#include <optional>
#include <string>

namespace engine {
    // Am tiet tieng Viet: [phu am dau][van][thanh dieu]
    // vd: "toan" -> initial="t", vowel="oa", coda="n", tone=0
    struct Syllable {
        std::u32string initial; // phu am dau: t, tr, ng, ngh...
        std::u32string vowel; // nguyen am: a, oa, uye...
        std::u32string coda; // phu am cuoi: n, ng, c, t...
        int tone = 0; // 0=ngang 1=sac 2=huyen 3=hoi 4=nga 5=nang

        // Phan tich buffer hien tai thanh am tiet; nullopt neu khong hop le
        static std::optional<Syllable> parse(std::u32string_view text);

        // Kiem tra cau truc am tiet co the la tieng Viet khong
        [[nodiscard]] bool valid() const;

        // Ghep lai thanh chuoi hoan chinh, dat dau theo kieu cu/moi
        [[nodiscard]] std::u32string compose(bool newToneStyle) const;

        // Vi tri nguyen am nhan dau thanh
        [[nodiscard]] int tonePosition(bool newToneStyle) const;
    };
} // namespace engine
