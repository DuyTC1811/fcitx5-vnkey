#pragma once
#include <optional>
#include <string>
#include <vector>

namespace engine {
    // Am tiet tieng Viet: [phu am dau][van][thanh dieu]
    // initial/vowel/coda LUON chua lowercase — case goc luu o cac co up* song song,
    // va CHI duoc ap dung mot cho duy nhat: cuoi compose().
    struct Syllable {
        std::u32string initial; // phu am dau: t, tr, ng, ngh...
        std::u32string vowel; // nguyen am: a, oa, uye...
        std::u32string coda; // phu am cuoi: n, ng, c, t...
        std::vector<bool> upInitial;
        std::vector<bool> upVowel;
        std::vector<bool> upCoda;
        int tone = 0; // 0=ngang 1=sac 2=huyen 3=hoi 4=nga 5=nang

        // Helper giu ky tu va co case luon dong bo
        void pushInitial(char32_t c, bool upper);

        void pushVowel(char32_t c, bool upper);

        void pushCoda(char32_t c, bool upper);

        void popBack(); // xoa ky tu cuoi: coda -> vowel -> initial; het vowel thi mat tone

        // Kiem tra cau truc am tiet co the la tieng Viet khong
        [[nodiscard]] bool valid() const;

        // Ghep lai thanh chuoi hoan chinh, dat dau theo kieu cu/moi
        [[nodiscard]] std::u32string compose(bool newToneStyle) const;

        // Vi tri nguyen am nhan dau thanh
        [[nodiscard]] int tonePosition(bool newToneStyle) const;
    };
} // namespace engine
