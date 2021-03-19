#pragma once
#include <bit>
#include <array>
#include <cstdint>
#include <cstdio>
#include <type_traits>

namespace gb {

    using byte_t = std::uint8_t;
    using sbyte_t = std::int8_t;
    using word_t = std::uint16_t;
    using sword_t = std::int16_t;

    struct BUS;
    struct CPU;
    struct MCB1;
    struct MCB2;
    struct MCB3;
}

