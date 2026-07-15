#include "test_helpers.h"

void test_engine_basics();

void test_telex();

int main() {
    std::printf("╔════════════════════════════════════════╗\n");
    std::printf("║          FCITX5-VNKEY TEST SUITE       ║\n");
    std::printf("╚════════════════════════════════════════╝\n");

    test_engine_basics();
    test_telex();

    printTestSummary();
    return g_failures > 0 ? 1 : 0;
}
