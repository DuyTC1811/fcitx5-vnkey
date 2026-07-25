#include "engine.h"
#include "input_method.h"
#include "test_helpers.h"

void test_telex() {
    using namespace engine;

    std::printf("\n--- TELEX: 5 THANH ĐIỆU ---\n");
    {
        TELEX("as", "á", "as >>> á");
        TELEX("af", "à", "af >>> à");
        TELEX("ar", "ả", "ar >>> ả");
        TELEX("ax", "ã", "ax >>> ã");
        TELEX("aj", "ạ", "aj >>> ạ");
    }

    std::printf("\n--- TELEX: DẤU CHỮ ---\n");
    {
        TELEX("aa", "â", "aa >>> â");
        TELEX("aw", "ă", "aw >>> ă");
        TELEX("oo", "ô", "oo >>> ô");
        TELEX("ow", "ơ", "ow >>> ơ");
        TELEX("uw", "ư", "uw >>> ư");
        TELEX("ee", "ê", "ee >>> ê");
        TELEX("dd", "đ", "dd >>> đ");
        TELEX("aaa", "aa", "gõ lặp 3 lần -> huỷ giấu aaa >>> 'aa'");
    }
    std::printf("\n--- TELEX: DẤU CHỮ + THANH ĐIỆU ---\n");
    {
        TELEX("aas", "ấ", "aas >>> ấ");
        TELEX("aws", "ắ", "aws >>> ắ");
        TELEX("ows", "ớ", "ows >>> ớ");
        TELEX("gocso", "gốc", "gocso >>> 'gốc'");
        TELEX("gocos", "gốc", "gocos >>> 'gốc' (dao thu tu s/o)");
        TELEX("goocs", "gốc", "goocs >>> 'gốc' (oo truoc coda)");
        TELEX("hocj", "học", "hocj >>> 'học'");
        TELEX("nhatas", "nhất", "nhatas >>> 'nhất' (a sau coda t)");
    }

    std::printf("\n--- TELEX: TỪ HOÀN CHỈNH ---\n");
    {
        TELEX("toans", "toán", "toans >>> 'toán'");
        TELEX("hoas", "hoá", "hoas >>> 'hoá' [Không phải 'hóa']");
        TELEX("hocj", "học", "hocj >>> 'học'");
        TELEX("ngoaij", "ngoại", "ngoaij >>> ngoại");
        TELEX("quar", "quả", "quar >>> 'quả");
        TELEX("ddoongf", "đồng", "ddoongf >>> 'đồng'");
        TELEX("Vieetj", "Việt", "Vieetj >>> 'Việt'");
        TELEX("dduwowcj", "được", "dduwowcj >>> 'được'");
        TELEX("nguwowfi", "người", "nguwowfi>>> 'người'");
        TELEX("tieengs", "tiếng", "tieengs >>> 'tiếng'");
        TELEX("huowngs", "hướng", "huowngs >>> 'hướng'");
        TELEX("nguowif", "người", "nguowif >>> 'người'");
        TELEX("quowr", "quở", "quowr>>> 'quở'");
        TELEX("tieengs", "tiếng", "tieengs >>> 'tiếng'");
        TELEX("VIEETJ", "VIỆT", "[CapsLock]: VIEETJ >>> 'VIỆT'");
        TELEX("DDoongf", "Đồng", "DDoongf >>> 'Đồng'");
        TELEX("Hoaf", "Hoà", "Hoaf >>> 'Hoà'");
    }

    std::printf("\n--- TELEX: HORN EDGE CASES ---\n");
    {
        InputProcessor p(makeTelex());
        TELEX("uwaw", "uaw", "uwaw >>>'uaw'");
        TELEX("huowng","hương", "huowng >>> 'huowng'");
        TELEX("huowngw","huowng", "huowngw >>> 'huowng'");
        TELEX("quowfn", "quờn", "quowfn >>> 'quờn'");
        TELEX("ruawr", "rửa", "ruawr >>> 'rửa'");
        TELEX("muaw", "mưa", "muaw >>> 'mưa'");
        TELEX("cuawr", "cửa", "cuawr >>> 'cửa'");
        TELEX("duwaf", "dừa", "duwaf >>> 'dừa'"); // w truoc a: uw -> ư roi + a
        TELEX("chuaws", "chứa", "chuaws >>> 'chứa'");
        TELEX("giuawx", "giữa", "giuawx >>> 'giữa'");
        TELEX("quawng", "quăng", "quawng >>> 'quăng'");
        TELEX("quawns", "quắn", "quawns >>> 'quắn'");
        TELEX("xoawn", "xoăn", "xoawn >>> 'xoăn'");
        TELEX("huowng", "hương", "huowng >>> 'hương'");
        TELEX("dduwowcj", "được", "dduwowcj >>> 'được'");
        TELEX("muaww", "muaw", "muaww >>> 'muaw' (huy horn)");
        TELEX("kaffka", "kafka", "kafka >>> 'kafka");
        TELEX("Kaffka", "Kafka", "Kafka >>> 'Kafka");
    }
}
