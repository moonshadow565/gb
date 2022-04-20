#pragma once
#include "cpu.hpp"

struct gb::CPU::BUS {
    gb_func virtual read_byte(word_t address) noexcept->byte_t = 0;
    gb_func virtual write_byte(word_t address, byte_t value) noexcept->void = 0;
    gb_func virtual waste() noexcept->void = 0;
};
