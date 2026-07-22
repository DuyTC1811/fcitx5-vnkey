#include "engine.hpp"
#include "input_method.hpp"
#include "test_helpers.h"

void test_telex() {
    using namespace engine;

    std::printf("\n--- TELEX: phim thuong ---\n");
    {
        InputProcessor p(makeTelex());
        CHECK_EQ(feed(p, "abc"), "abc", "Append: 'abc'");
    }
    {
        InputProcessor p(makeTelex());
        CHECK_EQ(feed(p, "nguoi"), "nguoi", "Chua co dau: 'nguoi'");
    }

    // NHOM 2: 5 THANH DIEU — can compose() + tonePosition() dung
    std::printf("\n--- TELEX: 5 THANH ĐIỆU ---\n");
    {
        InputProcessor p(makeTelex());
        CHECK_EQ(feed(p, "as"), "á", "s -> á");
    }
    {
        InputProcessor p(makeTelex());
        CHECK_EQ(feed(p, "af"), "à", "f -> à");
    }
    {
        InputProcessor p(makeTelex());
        CHECK_EQ(feed(p, "ar"), "ả", "r -> ả");
    }
    {
        InputProcessor p(makeTelex());
        CHECK_EQ(feed(p, "ax"), "ã", "x -> ã");
    }
    {
        InputProcessor p(makeTelex());
        CHECK_EQ(feed(p, "aj"), "ạ", "j -> ạ");
    }

    std::printf("\n--- TELEX: VỊ TRÍ ĐẶT DẤU ---\n");
    {
        InputProcessor p(makeTelex());
        CHECK_EQ(feed(p, "toans"), "toán", "Dấu nguyên âm chính: 'toán'");
    }
    {
        InputProcessor p(makeTelex());
        CHECK_EQ(feed(p, "hoas"), "hoá", "Kiểu mới: 'hoá' (Không phải 'hóa')");
    }
    {
        InputProcessor p(makeTelex());
        CHECK_EQ(feed(p, "hocj"), "học", "có phụ âm cuối: 'học'");
    }
    {
        InputProcessor p(makeTelex());
        CHECK_EQ(feed(p, "ngoaij"), "ngoại", "Phu am dau 2 chu + nguyen am doi");
    }

    // aa->â aw->ă oo->ô ow->ơ uw->ư dd->đ ee->ê
    std::printf("\n--- TELEX: DẤU CHỮ ---\n");
    {
        InputProcessor p(makeTelex());
        CHECK_EQ(feed(p, "aa"), "â", "aa -> â");
    }
    {
        InputProcessor p(makeTelex());
        CHECK_EQ(feed(p, "aw"), "ă", "aw -> ă");
    }
    {
        InputProcessor p(makeTelex());
        CHECK_EQ(feed(p, "oo"), "ô", "oo -> ô");
    }
    {
        InputProcessor p(makeTelex());
        CHECK_EQ(feed(p, "ow"), "ơ", "ow -> ơ");
    }
    {
        InputProcessor p(makeTelex());
        CHECK_EQ(feed(p, "uw"), "ư", "uw -> ư");
    }
    {
        InputProcessor p(makeTelex());
        CHECK_EQ(feed(p, "ee"), "ê", "ee -> ê");
    }
    {
        InputProcessor p(makeTelex());
        CHECK_EQ(feed(p, "dd"), "đ", "dd -> đ");
    }
    {
        InputProcessor p(makeTelex());
        CHECK_EQ(feed(p, "aaa"), "aa", "Go lap 3 lan -> huy dau chu, tra 'aa'");
    }

    std::printf("\n--- TELEX: DẤU CHỮ + THANH ĐIỆU ---\n");
    {
        InputProcessor p(makeTelex());
        CHECK_EQ(feed(p, "aas"), "ấ", "â + sac >>> ấ");
    }
    {
        InputProcessor p(makeTelex());
        CHECK_EQ(feed(p, "aws"), "ắ", "ă + sac >>> ắ");
    }
    {
        InputProcessor p(makeTelex());
        CHECK_EQ(feed(p, "ows"), "ớ", "ơ + sac >>> ớ");
    }

    std::printf("\n--- TELEX: TỪ HOÀN CHỈNH ---\n");
    {
        InputProcessor p(makeTelex());
        CHECK_EQ(feed(p, "ddoongf"), "đồng", "dd + oo + f >>> 'đồng'");
    }
    {
        InputProcessor p(makeTelex());
        CHECK_EQ(feed(p, "Vieetj"), "Việt", "Vi + ee + j >>> 'Việt'");
    }
    {
        InputProcessor p(makeTelex());
        CHECK_EQ(feed(p, "dduwowcj"), "được", "dd + uw + ow + j >>> 'được'");
    }
    {
        InputProcessor p(makeTelex());
        CHECK_EQ(feed(p, "nguwowfi"), "người", "n + g + u + w + o + w + f + i >>> 'người'");
    }
    {
        InputProcessor p(makeTelex());
        CHECK_EQ(feed(p, "tieengs"), "tiếng", "t + i + ee + n + g + s >>> 'tiếng'");
    }
    {
        InputProcessor p(makeTelex());
        CHECK_EQ(feed(p, "huowngs"), "hướng", "huowngs >>> 'hướng'");
    }
    {
        InputProcessor p(makeTelex());
        CHECK_EQ(feed(p, "nguowif"), "người", "nguowif >>> 'người'");
    }
    {
        InputProcessor p(makeTelex());
        CHECK_EQ(feed(p, "quowr"), "quở", "quowr>>> 'quở'");
    }
    {
        InputProcessor p(makeTelex());
        CHECK_EQ(feed(p, "tieengs"), "tiếng", "t + i + ee + n + g + s >>> 'tiếng'");
    }
    {
        InputProcessor p(makeTelex());
        CHECK_EQ(feed(p, "VIEETJ"), "VIỆT", "CapsLock: V + I + EE + T + J >>> 'VIỆT'");
    }
    {
        InputProcessor p(makeTelex());
        CHECK_EQ(feed(p, "DDoongf"), "Đồng", "hoa xen ke: DD + o + o + n + g + f >>> 'Đồng'");
    }
    {
        InputProcessor p(makeTelex());
        CHECK_EQ(feed(p, "Hoaf"), "Hoà", "H + o + a + f >>> 'Hoà' (giu chu hoa dau)");
    }
    {
        InputProcessor p(makeTelex());
        // feedSentence = noi cac r.text (COMMIT) + p.preedit() cuoi cung.
        // Tu cuoi "nam" chua commit (khong co space sau no) -> nam trong preedit.
        CHECK_EQ(feedSentence(p, "coongj hoaf xax hooij chur nghiax vieejt nam"), "cộng hoà xã hội chủ nghĩa việt nam",
                 "cau day du: cac tu commit kem space, tu cuoi con trong preedit");
    }
    {
        InputProcessor p(makeTelex());
        // Co space cuoi -> moi tu deu commit, ket qua co space ket thuc
        CHECK_EQ(feedSentence(p, "coongj hoaf xax "), "cộng hoà xã ",
                 "cau ket thuc bang space: commit het, co space cuoi");
    }

    // ---- 2. SPACE khi buffer rong: passthrough ----
    {
        InputProcessor p(makeTelex());
        KeyInput sp;
        sp.special = KeyInput::Special::SPACE;
        auto r = p.process(sp);
        CHECK_EQ_INT(r.action, Action::PASS_THROUGH, "Space buffer rong: PASS_THROUGH");
    }

    // ---- 3. Hai space lien tiep: commit roi passthrough ----
    {
        InputProcessor p(makeTelex());
        KeyInput k;
        k.ch = U'a';
        p.process(k);
        KeyInput sp;
        sp.special = KeyInput::Special::SPACE;
        auto r1 = p.process(sp);
        CHECK_EQ(r1.text, "a ", "Space 1: commit 'a '");
        auto r2 = p.process(sp);
        CHECK_EQ_INT(r2.action, Action::PASS_THROUGH, "Space 2: buffer rong -> PASS_THROUGH");
    }

    // ---- 4. ENTER: commit KHONG kem newline, forwardKey = true ----
    {
        InputProcessor p(makeTelex());
        for (char c: {'m', 'a', 's'}) { // "mas" -> "má"
            KeyInput k;
            k.ch = static_cast<char32_t>(c);
            p.process(k);
        }
        KeyInput en;
        en.special = KeyInput::Special::ENTER;
        auto r = p.process(en);
        CHECK_EQ_INT(r.action, Action::COMMIT, "Enter: action = COMMIT");
        CHECK_EQ(r.text, "má", "Enter: commit 'má' khong kem ky tu them");
        CHECK_EQ_INT(r.forwardKey, true, "Enter: forwardKey = true (app phai nhan Enter)");
        CHECK_EQ(p.preedit(), "", "Enter: buffer reset");
    }

    // ---- 5. TAB: nhu Enter ----
    {
        InputProcessor p(makeTelex());
        KeyInput k;
        k.ch = U'a';
        p.process(k);
        KeyInput tab;
        tab.special = KeyInput::Special::TAB;
        auto r = p.process(tab);
        CHECK_EQ_INT(r.action, Action::COMMIT, "Tab: action = COMMIT");
        CHECK_EQ_INT(r.forwardKey, true, "Tab: forwardKey = true");
    }

    // ---- 6. ENTER buffer rong: passthrough, khong forward flag ----
    {
        InputProcessor p(makeTelex());
        KeyInput en;
        en.special = KeyInput::Special::ENTER;
        auto r = p.process(en);
        CHECK_EQ_INT(r.action, Action::PASS_THROUGH, "Enter buffer rong: PASS_THROUGH");
    }

    // ---- 7. ESCAPE: commit chuoi phim tho, KHONG forward ----
    {
        InputProcessor p(makeTelex());
        for (char c: {'m', 'a', 's'}) {
            KeyInput k;
            k.ch = static_cast<char32_t>(c);
            p.process(k);
        }
        KeyInput esc;
        esc.special = KeyInput::Special::ESCAPE;
        auto r = p.process(esc);
        CHECK_EQ_INT(r.action, Action::COMMIT, "Esc: action = COMMIT");
        CHECK_EQ(r.text, "mas", "Esc: commit chuoi tho 'mas' (huy che bien)");
        CHECK_EQ_INT(r.forwardKey, false, "Esc: khong forward (nuot phim)");
    }


    // ---- 9. Space ngay sau commit bang space: chi ra 1 khoang trang moi lan ----
    {
        InputProcessor p(makeTelex());
        KeyInput k;
        k.ch = U'a';
        KeyInput sp;
        sp.special = KeyInput::Special::SPACE;
        p.process(k);
        auto r1 = p.process(sp);
        p.process(k);
        auto r2 = p.process(sp);
        CHECK_EQ(r1.text + r2.text, "a a ", "2 tu lien tiep: 'a a ' dung 2 space");
    }

    // ========================================================================
    // NHOM: BACKSPACE REPLAY — raw_/syl_ khong duoc lech
    // ========================================================================
    std::printf("\n--- TELEX: BACKSPACE REPLAY ---\n");

    // Backspace sau phim tone: bo dau, KHONG mat chu
    {
        InputProcessor p(makeTelex());
        feed(p, "mas"); // "má"
        feedSpecial(p, KeyInput::Special::BACKSPACE);
        CHECK_EQ(p.preedit(), "ma", "BS sau 'mas': bo dau sac -> 'ma' (khong mat 'a')");
    }

    // Backspace sau dau chu: go lai duoc tu dau
    {
        InputProcessor p(makeTelex());
        feed(p, "ddoongf"); // "đồng"
        feedSpecial(p, KeyInput::Special::BACKSPACE); // bo 'f' -> "đông"
        CHECK_EQ(p.preedit(), "đông", "BS sau 'ddoongf': bo thanh huyen -> 'đông'");
        feedSpecial(p, KeyInput::Special::BACKSPACE); // bo 'g' -> "đôn"
        CHECK_EQ(p.preedit(), "đôn", "BS tiep: bo 'g' -> 'đôn'");
    }

    // Xoa het khong crash, khong ton phim ma trong raw_
    {
        InputProcessor p(makeTelex());
        feed(p, "coong"); // raw 5 phim, syl 4 ky tu "công"
        for (int i = 0; i < 8; ++i) { // xoa qua da
            feedSpecial(p, KeyInput::Special::BACKSPACE);
        }
        CHECK_EQ(p.preedit(), "", "BS qua da sau 'coong': buffer rong, khong crash");
        auto r = feedSpecial(p, KeyInput::Special::SPACE);
        CHECK_EQ_INT(r.action, Action::PASS_THROUGH, "Space sau khi xoa het: PASS_THROUGH (raw_ sach that su)");
    }

    // Backspace giu dung case (co up* replay dung)
    {
        InputProcessor p(makeTelex());
        feed(p, "Vieetj"); // "Việt"
        feedSpecial(p, KeyInput::Special::BACKSPACE); // bo 'j' -> "Viêt"
        CHECK_EQ(p.preedit(), "Viêt", "BS giu case: 'Việt' -> 'Viêt' (V van hoa)");
    }

    // Backspace roi go tiep: replay + phim moi phai khop
    {
        InputProcessor p(makeTelex());
        feed(p, "toanr"); // "toản"
        feedSpecial(p, KeyInput::Special::BACKSPACE); // bo 'r' -> "toan"
        feed(p, "s"); // go dau moi
        CHECK_EQ(p.preedit(), "toán", "BS roi go dau khac: 'toản' -> 'toan' -> 'toán'");
    }

    // Khong double-processing: go 1 phim ra dung 1 ky tu
    {
        InputProcessor p(makeTelex());
        feed(p, "a");
        CHECK_EQ(p.preedit(), "a", "1 phim 'a' -> dung 'a' (khong bi xu ly 2 lan)");
    }

    // ========================================================================
    // NHOM: PHIM DIEU KHIEN GIUA CHUNG — khong duoc treo preedit
    // ========================================================================
    std::printf("\n--- TELEX: PHIM DIEU KHIEN GIUA CHUNG ---\n");

    // Ctrl+key giua chung: chot chu + forward, KHONG vut buffer
    {
        InputProcessor p(makeTelex());
        feed(p, "mas"); // "má"
        KeyInput k;
        k.ch = U's';
        k.ctrlOrAlt = true;
        auto r = p.process(k);
        CHECK_EQ_INT(r.action, Action::COMMIT, "Ctrl+S giua chung: COMMIT (khong vut chu)");
        CHECK_EQ(r.text, "má", "Ctrl+S: commit 'má'");
        CHECK_EQ_INT(r.forwardKey, true, "Ctrl+S: forward de app nhan Ctrl+S");
        CHECK_EQ(p.preedit(), "", "Ctrl+S: buffer reset");
    }

    // Ctrl+key khi buffer rong: passthrough binh thuong
    {
        InputProcessor p(makeTelex());
        KeyInput k;
        k.ch = U'c';
        k.ctrlOrAlt = true;
        auto r = p.process(k);
        CHECK_EQ_INT(r.action, Action::PASS_THROUGH, "Ctrl+C buffer rong: PASS_THROUGH");
    }

    // Phim khong phai ky tu (mui ten...) giua chung: chot chu + forward
    {
        InputProcessor p(makeTelex());
        feed(p, "hoaf"); // "hoà"
        KeyInput arrow; // ch = 0, special = NONE (mui ten, Home...)
        auto r = p.process(arrow);
        CHECK_EQ_INT(r.action, Action::COMMIT, "Mui ten giua chung: COMMIT");
        CHECK_EQ(r.text, "hoà", "Mui ten: commit 'hoà'");
        CHECK_EQ_INT(r.forwardKey, true, "Mui ten: forward cho app di chuyen cursor");
    }

    // Delete khi khong co tone: chot chu + forward (app xoa ben phai)
    {
        InputProcessor p(makeTelex());
        feed(p, "toan"); // chua co dau
        auto r = feedSpecial(p, KeyInput::Special::DELETE);
        CHECK_EQ_INT(r.action, Action::COMMIT, "Delete khong tone: COMMIT + forward");
        CHECK_EQ_INT(r.forwardKey, true, "Delete khong tone: forwardKey = true");
    }

    // Delete khi co tone: bo tone, giu buffer
    {
        InputProcessor p(makeTelex());
        feed(p, "mas"); // "má"
        auto r = feedSpecial(p, KeyInput::Special::DELETE);
        CHECK_EQ_INT(r.action, Action::UPDATE_PREEDIT, "Delete co tone: UPDATE_PREEDIT");
        CHECK_EQ(p.preedit(), "ma", "Delete co tone: bo dau -> 'ma'");
    }

    // ESCAPE giu dung case phim tho da go
    {
        InputProcessor p(makeTelex());
        feed(p, "Mas"); // "Má"
        auto r = feedSpecial(p, KeyInput::Special::ESCAPE);
        CHECK_EQ(r.text, "Mas", "Esc: tra dung phim tho 'Mas' (giu hoa)");
    }

    // ========================================================================
    // NHOM: HORN EDGE CASES — go lap 'w'
    // ========================================================================
    std::printf("\n--- TELEX: HORN EDGE CASES ---\n");

    // "ưa" + w: phai HUY, khong duoc ra "ưă"
    {
        InputProcessor p(makeTelex());
        CHECK_EQ(feed(p, "uwaw"), "uaw", "uw+a+w: da co horn -> w thu 2 la huy, ra 'uaw'");
    }

    // "ương" + w: huy horn ca cap
    {
        InputProcessor p(makeTelex());
        feed(p, "huowngw"); // "hương" + w -> huy
        CHECK_EQ(p.preedit(), "huowng", "huowng+w: huy horn -> tra chuoi tho 'huowng'");
    }

    // qu + ow: chi horn 'o', khong dinh 'u'
    {
        InputProcessor p(makeTelex());
        CHECK_EQ(feed(p, "quowfn"), "quờn", "qu + ow + f + n: chi horn 'o' -> 'quờn'");
    }
}
