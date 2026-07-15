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

    // aa->â  aw->ă  oo->ô  ow->ơ  uw->ư  dd->đ  ee->ê
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
        CHECK_EQ(feedSentence(p, "coongj hoaf xax hooij chur ngiax vietej nam"), "cộng hoà xã hội chủ ngĩa việt nam", "từ khoá: 'cộng hoà xã hội chủ ngĩa việt nam'");
    }
}
