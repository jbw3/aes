/**
 * @file main.cpp
 * @author John Wilkes
 */

#include <iomanip>
#include <iostream>
#include <cstdint>
#include <cstring>

using std::uint32_t;
typedef unsigned int uint;

constexpr uint DATA_SIZE = 16;

const uint8_t S_BOX[] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16,
};

uint8_t sBoxSub(uint8_t b)
{
    int row = b >> 4;
    int col = b & 0x0F;
    return S_BOX[row * 16 + col];
}

uint32_t keyRotWord(uint32_t w)
{
    return (w << 8) | (w >> 24);
}

uint32_t keySubWord(uint32_t w)
{
    uint32_t out = 0;

    for (uint i = 0; i < sizeof(uint32_t); ++i)
    {
        uint8_t b = w >> (i * 8);
        uint32_t temp = sBoxSub(b);
        temp <<= i * 8;
        out |= temp;
    }

    return out;
}

/**
 * @param round 1 - 10]
 */
uint32_t keyRCon(int round)
{
    static const uint32_t RCON[] = {
        0x01000000,
        0x02000000,
        0x04000000,
        0x08000000,
        0x10000000,
        0x20000000,
        0x40000000,
        0x80000000,
        0x1B000000,
        0x36000000
    };
    return RCON[round - 1];
}

void keyExpansion(const uint32_t* key, uint32_t* w)
{
    // copy first 4 words to expanded key
    for (uint i = 0; i < 4; ++i)
    {
        w[i] = key[i];
    }

    for (uint i = 4; i < 44; ++i)
    {
        uint32_t temp = w[i - 1];
        if (i % 4 == 0)
        {
            temp = keyRotWord(temp);
            temp = keySubWord(temp);
            temp ^= keyRCon(i / 4);
        }

        w[i] = w[i - 4] ^ temp;
    }
}

void printKeyExpansion(const uint32_t* expKey)
{
    for (int i = 0; i < 44; i += 4)
    {
        for (int j = 0; j < 4; j++)
        {
            std::cout << std::setw(8) << std::setfill('0')
                      << expKey[i + j];
        }
        std::cout << '\n';
    }
}

void printKey(const uint32_t* key)
{
    std::cout << "  ";
    for (uint i = 0; i < 4; ++i)
    {
        std::cout << std::hex << std::setw(8) << std::setfill('0') << key[i];
    }
}

void printData(const uint8_t* data)
{
    std::cout << "  ";
    for (uint i = 0; i < DATA_SIZE; ++i)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<uint>(data[i]);
    }
}

void subBytes(uint8_t* data)
{
    for (uint i = 0; i < DATA_SIZE; ++i)
    {
        data[i] = sBoxSub(data[i]);
    }
}

void shiftRows(uint8_t* data)
{
    uint8_t temp1, temp2;

    temp1 = data[4];
    data[4] = data[5];
    data[5] = data[6];
    data[6] = data[7];
    data[7] = temp1;

    temp1 = data[8];
    temp2 = data[9];
    data[ 8] = data[10];
    data[ 9] = data[11];
    data[10] = temp1;
    data[11] = temp2;

    temp1 = data[15];
    data[15] = data[14];
    data[14] = data[13];
    data[13] = data[12];
    data[12] = temp1;
}

uint8_t times2(uint8_t n)
{
    uint8_t a = n << 1;
    if ((n & 0x80) != 0)
    {
        a ^= 0x1b;
    }
    return a;
}

uint8_t times3(uint8_t n)
{
    return n ^ times2(n);
}

void mixCols(uint8_t* data)
{
    uint8_t copy[DATA_SIZE];
    std::memcpy(copy, data, DATA_SIZE);

    for (uint c = 0; c < 4; ++c)
    {
        data[c] = times2(copy[c]) ^ times3(copy[4 + c]) ^ copy[8 + c] ^ copy[12 + c];
        data[4 + c] = copy[c] ^ times2(copy[4 + c]) ^ times3(copy[8 + c]) ^ copy[12 + c];
        data[8 + c] = copy[c] ^ copy[4 + c] ^ times2(copy[8 + c]) ^ times3(copy[12 + c]);
        data[12 + c] = times3(copy[c]) ^ copy[4 + c] ^ copy[8 + c] ^ times2(copy[12 + c]);
    }
}

void addRoundKey(const uint32_t* key, uint8_t* data)
{
    for (uint i = 0; i < 16; ++i)
    {
        uint8_t keyByte = key[i / 4] >> ((3 - (i % 4)) * 8);
        uint r = i % 4;
        uint c = i / 4;
        data[r * 4 + c] ^= keyByte;
    }
}

void aesRound(uint round, const uint32_t* key, uint8_t* data)
{
    subBytes(data);
    // printData(data);
    shiftRows(data);
    // printData(data);
    if (round < 10)
    {
        mixCols(data);
        printData(data);
    }
    addRoundKey(key, data);
}

void aes(const uint32_t* key, uint8_t* data)
{
    uint32_t expKey[44];
    keyExpansion(key, expKey);

    uint8_t state[DATA_SIZE];
    for (uint i = 0; i < DATA_SIZE; ++i)
    {
        uint r = i % 4;
        uint c = i / 4;
        state[r * 4 + c] = data[i];
    }

    std::cout << "Round  Round Key                         Result\n"
              << "-----  --------------------------------  --------------------------------\n";

    std::cout << "    0";
    printKey(&expKey[0]);

    addRoundKey(&expKey[0], state);

    printData(state);
    std::cout << '\n';

    for (uint round = 1; round <= 10; ++round)
    {
        const uint32_t* roundKey = &expKey[round * 4];

        std::cout << std::dec << std::setw(5) << std::setfill(' ') << round;
        printKey(roundKey);

        aesRound(round, roundKey, state);

        printData(state);
        std::cout << '\n';
    }
}

int main()
{
    std::cout << std::hex;

    // uint32_t key[4] = {0xFEDCBA98, 0x76543210, 0x01234567, 0x89ABCDEF};
    uint32_t key[4] = {0x0f1571c9, 0x47d9e859, 0x0cb7add6, 0xaf7f6798};
    uint8_t data[DATA_SIZE] = {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
    };

    aes(key, data);

    return 0;
}
