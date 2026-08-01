#include "engine.h"
#include "input_method.h"
#include "test_helpers.h"

// ============================================================================
// TEST ENGINE — kiem tra CO CHE cua InputProcessor, khong phai luat telex.
// (Luat go telex da co test_telex.cpp lo; o day dung telex chi lam vi du.)
//
// Pham vi:
//   1. toUtf8            — encode 1/2/3 byte
//   2. Action & Result   — dung action, dung forwardKey cho tung loai phim
//   3. Config            — newToneStyle, spellCheck bat/tat
//   4. Literal mode      — flat_ vs raw_, vao/ra literal
//   5. Backspace replay  — raw_/syl_/flat_/literal_ dong bo sau replay
//   6. Reset & vong doi  — buffer sach sau moi loai commit
// ============================================================================

void test_engine() {
    using namespace engine;


    std::printf("\n--- ENGINE: toUtf8 ---\n");
    CHECK_EQ(toUtf8(U"abc"), "abc", "toUtf8: ASCII giu nguyen");
    CHECK_EQ(toUtf8(U"đ"), "\xC4\x91", "toUtf8: 2-byte (đ = U+0111)");
    CHECK_EQ(toUtf8(U"ế"), "\xE1\xBA\xBF", "toUtf8: 3-byte (ế = U+1EBF)");
    CHECK_EQ(toUtf8(U""), "", "toUtf8: chuoi rong");
    CHECK_EQ(toUtf8(U"Việt"), "Vi\xE1\xBB\x87t", "toUtf8: chuoi tron ASCII + 3-byte");

    std::printf("\n--- ENGINE: ACTION CO BAN ---\n");
    {
        InputProcessor p(makeTelex());
        auto r = feedKey(p, 'a');
        CHECK_EQ_INT(r.action, Action::UPDATE_PREEDIT, "phim chu: UPDATE_PREEDIT");
        CHECK_EQ(r.text, "a", "phim chu: text = preedit hien tai");
        CHECK_EQ_INT(r.forwardKey, false, "phim chu: khong forward");
    }
    {
        InputProcessor p(makeTelex());
        // Moi phim dac biet khi buffer RONG deu phai PASS_THROUGH
        for (const auto sp: {
                     KeyInput::Special::BACKSPACE, //
                     KeyInput::Special::DELETE, //
                     KeyInput::Special::SPACE, //
                     KeyInput::Special::ENTER, //
                     KeyInput::Special::TAB, //
                     KeyInput::Special::ESCAPE //
             }) {
            auto r = feedSpecial(p, sp);
            CHECK_EQ_INT(r.action, Action::PASS_THROUGH, "special + buffer rong: PASS_THROUGH");
        }
    }
    {
        InputProcessor p(makeTelex());
        KeyInput arrow; // ch = 0, special = NONE
        auto r = p.process(arrow);
        CHECK_EQ_INT(r.action, Action::PASS_THROUGH, "arrow + buffer rong: PASS_THROUGH");
    }

    std::printf("\n--- ENGINE: forwardKey THEO TUNG PHIM ---\n");
    {
        // Bang ky vong: phim nao commit xong PHAI tra phim goc cho app?
        //   Enter/Tab/Ctrl/Arrow -> forward = true
        //   Space/Escape         -> forward = false (nuot phim)
        InputProcessor p(makeTelex());
        feed(p, "a");
        CHECK_EQ_INT(feedSpecial(p, KeyInput::Special::ENTER).forwardKey, true, "Enter: forward");
        feed(p, "a");
        CHECK_EQ_INT(feedSpecial(p, KeyInput::Special::TAB).forwardKey, true, "Tab: forward");
        feed(p, "a");
        CHECK_EQ_INT(feedSpecial(p, KeyInput::Special::SPACE).forwardKey, false, "Space: khong forward");
        feed(p, "a");
        CHECK_EQ_INT(feedSpecial(p, KeyInput::Special::ESCAPE).forwardKey, false, "Escape: khong forward");
    }
    {
        InputProcessor p(makeTelex());
        feed(p, "a");
        KeyInput ctrl;
        ctrl.ch = U'c';
        ctrl.ctrlOrAlt = true;
        CHECK_EQ_INT(p.process(ctrl).forwardKey, true, "Ctrl+key: forward");
    }
    {
        InputProcessor p(makeTelex());
        feed(p, "a");
        KeyInput arrow;
        CHECK_EQ_INT(p.process(arrow).forwardKey, true, "Arrow: forward");
    }

    std::printf("\n--- ENGINE: CONFIG newToneStyle ---\n");
    {
        InputProcessor p(makeTelex(), Config{.newToneStyle = true});
        CHECK_EQ(feed(p, "hoas"), "hoá", "newToneStyle=true: 'hoá'");
    }
    {
        InputProcessor p(makeTelex(), Config{.newToneStyle = false});
        CHECK_EQ(feed(p, "hoas"), "hóa", "newToneStyle=false: 'hóa'");
    }

    std::printf("\n--- ENGINE: CONFIG spellCheck ---\n");
    {
        InputProcessor p(makeTelex(), Config{.spellCheck = true});
        CHECK_EQ(feed(p, "kafk"), "kafk", "spellCheck=true: coda 'fk' vo -> literal, hien raw gop");
    }
    {
        InputProcessor p(makeTelex(), Config{.spellCheck = false});
        CHECK_EQ(feed(p, "kafk"), "kàk", "spellCheck=false: khong literal ('f' van la thanh huyen)");
    }

    std::printf("\n--- ENGINE: LITERAL MODE — flat_ vs raw_ ---\n");
    {
        InputProcessor p(makeTelex());
        feed(p, "kaffka");
        CHECK_EQ(p.preedit(), "kafka", "literal: preedit = flat (ff gop con 1 f)");
        auto r = feedSpecial(p, KeyInput::Special::ESCAPE);
        CHECK_EQ(r.text, "kaffka", "literal + Esc: commit raw_ day du (khong phai flat)");
    }
    {
        InputProcessor p(makeTelex());
        CHECK_EQ(feed(p, "kafka"), "kafka", "literal: tone 'f' hien lai trong flat");
    }
    {
        InputProcessor p(makeTelex());
        feed(p, "kafk"); // literal
        feed(p, "s"); // 's' phai la ky tu thuong, khong phai dau sac
        CHECK_EQ(p.preedit(), "kafks", "trong literal: 's' append tho, khong thanh dau");
    }
    {
        // Case hoa bao toan trong literal (flat giu phim goc)
        InputProcessor p(makeTelex());
        CHECK_EQ(feed(p, "KAFKA"), "KAFKA", "literal: giu nguyen hoa");
    }

    std::printf("\n--- ENGINE: LITERAL + SPACE COMMIT ---\n");
    {
        InputProcessor p(makeTelex());
        feed(p, "kafka");
        auto r = feedSpecial(p, KeyInput::Special::SPACE);
        CHECK_EQ_INT(r.action, Action::COMMIT, "literal + Space: COMMIT");
        CHECK_EQ(r.text, "kafka ", "literal + Space: commit flat + ' '");
        CHECK_EQ(p.preedit(), "", "literal + Space: buffer reset");
    }
    {
        // Sau commit literal, tu tiep theo phai go binh thuong (literal_ da reset)
        InputProcessor p(makeTelex());
        feedSpecial(p, KeyInput::Special::SPACE); // no-op
        feed(p, "kafka");
        feedSpecial(p, KeyInput::Special::SPACE);
        CHECK_EQ(feed(p, "mas"), "má", "sau commit literal: tu moi transform binh thuong");
    }

    std::printf("\n--- ENGINE: BACKSPACE REPLAY DONG BO 4 TRANG THAI ---\n");
    {
        // Vao literal roi backspace lui ve truoc diem vo -> PHAI thoat literal
        InputProcessor p(makeTelex());
        feed(p, "kafk"); // literal (coda "fk")
        feedSpecial(p, KeyInput::Special::BACKSPACE); // replay "kaf"
        CHECK_EQ(p.preedit(), "kà", "BS thoat literal: 'kaf' compose lai thanh 'kà'");
        feed(p, "s");
        CHECK_EQ(p.preedit(), "ká", "sau khi thoat literal: transform hoat dong lai");
    }
    {
        // Replay giu flat_ dung: xoa 1 phim cua kaffka
        InputProcessor p(makeTelex());
        feed(p, "kaffka"); // raw "kaffka"
        feedSpecial(p, KeyInput::Special::BACKSPACE); // replay "kaffk" -> van literal
        CHECK_EQ(p.preedit(), "kafk", "BS trong literal: flat rebuild dung (ff van gop)");
    }
    {
        // Xoa den sach: khong crash, trang thai that su rong
        InputProcessor p(makeTelex());
        feed(p, "kafka");
        for (int i = 0; i < 10; ++i) {
            feedSpecial(p, KeyInput::Special::BACKSPACE);
        }
        CHECK_EQ(p.preedit(), "", "BS qua da trong literal: buffer rong");
        CHECK_EQ_INT(feedSpecial(p, KeyInput::Special::SPACE).action, Action::PASS_THROUGH,
                     "sau khi xoa het: Space PASS_THROUGH (raw_ sach)");
    }

    std::printf("\n--- ENGINE: VONG DOI BUFFER SAU COMMIT ---\n");
    {
        // Moi duong commit deu phai de lai buffer sach
        InputProcessor p(makeTelex());
        feed(p, "mas");
        feedSpecial(p, KeyInput::Special::ENTER);
        CHECK_EQ(p.preedit(), "", "sau Enter: sach");
        feed(p, "mas");
        feedSpecial(p, KeyInput::Special::SPACE);
        CHECK_EQ(p.preedit(), "", "sau Space: sach");
        feed(p, "mas");
        feedSpecial(p, KeyInput::Special::ESCAPE);
        CHECK_EQ(p.preedit(), "", "sau Escape: sach");
        feed(p, "mas");
        p.reset();
        CHECK_EQ(p.preedit(), "", "sau reset(): sach");
    }
    {
        // Commit khong de "di chung" trang thai sang tu sau (bug tone dinh lai)
        InputProcessor p(makeTelex());
        feed(p, "mas"); // tone sac
        feedSpecial(p, KeyInput::Special::SPACE);
        CHECK_EQ(feed(p, "ma"), "ma", "tu sau khong thua huong tone cua tu truoc");
    }

    std::printf("\n--- ENGINE: DELETE ---\n");
    {
        InputProcessor p(makeTelex());
        feed(p, "mas"); // co tone
        auto r1 = feedSpecial(p, KeyInput::Special::DELETE);
        CHECK_EQ_INT(r1.action, Action::UPDATE_PREEDIT, "Delete co tone: chi bo tone");
        CHECK_EQ(p.preedit(), "ma", "Delete co tone: 'má' -> 'ma'");
        auto r2 = feedSpecial(p, KeyInput::Special::DELETE);
        CHECK_EQ_INT(r2.action, Action::COMMIT, "Delete lan 2 (het tone): COMMIT + forward");
        CHECK_EQ_INT(r2.forwardKey, true, "Delete lan 2: forwardKey = true");
    }
}
