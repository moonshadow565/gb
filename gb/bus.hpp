#pragma once
#include "types.hpp"

struct gb::BUS {
    constexpr virtual byte_t read_byte(word_t address) noexcept = 0;
    constexpr virtual void write_byte(word_t address, byte_t value) noexcept = 0;
    constexpr virtual void waste() noexcept = 0;
};
