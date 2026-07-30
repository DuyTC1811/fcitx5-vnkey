#pragma once
#include <optional>
#include <string>
#include <vector>

namespace engine {
    // ÂM TIẾT TIẾNG VIỆT : [PHỤ ÂM GIẤU][van][THANH ĐIỆU]
    // initial/vowel/coda LUON chua lowercase — case goc luu o cac co up* song song,
    // va CHI duoc ap dung mot cho duy nhat: cuoi compose().
    struct Syllable {
        std::u32string initial; // PHỤ ÂM GIẤU: t, tr, ng, ngh...
        std::u32string vowel; // NGUYÊN ÂM: a, oa, uye...
        std::u32string coda; // PHỤ ÂM CUỐI: n, ng, c, t...
        std::vector<bool> upInitial;
        std::vector<bool> upVowel;
        std::vector<bool> upCoda;
        int tone = 0; // 0 = NGÃ 1 = SẮC 2 = HUYỀN 3 = HỎI 4 = MGÃ 5 = NẶNG

        // Helper giu ky tu va co case luon dong bo
        void pushInitial(char32_t c, bool upper);

        void pushVowel(char32_t c, bool upper);

        void pushCoda(char32_t c, bool upper);

        void popBack(); // xoa ky tu cuoi: coda -> vowel -> initial; het vowel thi mat tone

        // Am tiet da vo cau truc (coda chua nguyen am) -> chi con la chuoi tho
        [[nodiscard]] bool broken() const;

        // Kiem tra cau truc am tiet co the la tieng Viet khong
        [[nodiscard]] bool valid() const;

        // Ghep lai thanh chuoi hoan chinh, dat dau theo kieu cu/moi
        [[nodiscard]] std::u32string compose(bool newToneStyle) const;

        // Vi tri nguyen am nhan dau thanh
        [[nodiscard]] int tonePosition(bool newToneStyle) const;

        // Cau truc co the la tieng Viet khong (coda hop le)? Dung cho spellcheck
        [[nodiscard]] bool structureOk() const;

        // Coda tac (p t c ch) BUOC phai mang thanh sac hoac nang: "hoc" khong ton tai
        [[nodiscard]] bool toneOk() const;
    };
} // namespace engine
