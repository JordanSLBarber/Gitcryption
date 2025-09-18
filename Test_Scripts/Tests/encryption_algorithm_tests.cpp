#include <iostream>
#include <cassert>
#include "../Src/encryption_tests.h"

void testRotateWord() {
    uint32_t input = 0xAABBCCDD;
    uint32_t expected = 0xBBCCDDAA;
    assert(rotateWord(input) == expected);

    input = 0x12345678;
    expected = 0x34567812;
    assert(rotateWord(input) == expected);

    input = 0x00000011;
    expected = 0x00001100;
    assert(rotateWord(input) == expected);

    input = 0x11000000;
    expected = 0x00000011;
    assert(rotateWord(input) == expected);
    std::cout << "rotateWord tests passed." << std::endl;
}

int test_sBoxForward() {
    uint8_t input = 0x53;
    uint8_t expected = 0xED; // Example expected value, replace with actual S-box value
    assert(sBoxForward(input) == expected);

    input = 0x00;
    expected = 0x63; // Example expected value, replace with actual S-box value
    assert(sBoxForward(input) == expected);

    input = 0xFF;
    expected = 0x16; // Example expected value, replace with actual S-box value
    assert(sBoxForward(input) == expected);
    std::cout << "sBoxForward tests passed." << std::endl;
    return 0;
}

int test_sBoxReverse(){
    uint8_t input = 0xED;
    uint8_t expected = 0x53; // Example expected value, replace with actual inverse S-box value
    assert(sBoxReverse(input) == expected);

    input = 0x63;
    expected = 0x00; // Example expected value, replace with actual inverse S-box value
    assert(sBoxReverse(input) == expected);

    input = 0x16;
    expected = 0xFF; // Example expected value, replace with actual inverse S-box value
    assert(sBoxReverse(input) == expected);
    std::cout << "sBoxReverse tests passed." << std::endl;
    return 0;
}

int test_sBoxSynchronous(){
    for (uint32_t i = 0; i <= 256; ++i) {
        assert(sBoxReverse(sBoxForward(static_cast<uint8_t>(i))) == static_cast<uint8_t>(i));
    }
    std::cout << "S-box forward and reverse synchronization test passed." << std::endl;
    return 0;
}

int main() {
    testRotateWord();
    test_sBoxForward();
    test_sBoxReverse();
    test_sBoxSynchronous();
    std::cout << "All tests passed successfully." << std::endl;
    return 0;
}