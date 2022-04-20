#pragma once
#include <cstdio>

#include "cpu_bus.hpp"

struct gb::CPU::MCB1 final : gb::CPU::BUS {
    std::array<byte_t, 0x140000> ROM = {};
    std::array<byte_t, 0x2000> VRAM = {};
    std::array<byte_t, 0x8000> WRAM = {};
    std::array<byte_t, 0x8000> ERAM = {};
    std::array<byte_t, 0x80> HRAM = {};

    bool eram_enable = {};
    bool mode = {};
    byte_t rom_bank = {};
    byte_t eram_bank = 1;
    byte_t wram_bank = {};
    char serial = {};

    gb_func virtual read_byte(word_t address) noexcept->byte_t override {
        switch ((address >> 12) & 0xF) {
            case 0x0:
            case 0x1:
            case 0x2:
            case 0x3:
                return ROM[address & 0x3FFF];
            case 0x4:
            case 0x5:
            case 0x6:
            case 0x7:
                return ROM[(rom_bank * 0x4000) + (address & 0x3FFF)];
            case 0x8:
            case 0x9:
                return VRAM[address & 0x1FFF];
            case 0xA:
            case 0xB:
                if (eram_enable) {
                    return ERAM[eram_bank * 0x2000 + (address & 0x1FFF)];
                } else {
                    return 0xFF;
                }
            case 0xC:
                return WRAM[address & 0xFFF];
            case 0xD:
                return WRAM[(wram_bank * 0x1000) + (address & 0x1FFF)];
            case 0xE:
                return WRAM[address & 0xFFF];
            case 0xF:
                if (address < 0xFE00) {
                    return WRAM[(wram_bank * 0x1000) + (address & 0x1FFF)];
                } else if (address == 0xFF44) {
                    return 0x90;
                } else if (address >= 0xFF80) {
                    return HRAM[address & 0x7F];
                }
                break;
        }
        return 0xFF;
    };

    gb_func virtual write_byte(word_t address, byte_t value) noexcept->void override {
        switch ((address >> 12) & 0xF) {
            case 0x0:
            case 0x1:
                eram_enable = (value & 0xF) == 0xA;
                break;
            case 0x2:
            case 0x3:
                rom_bank &= 0x60;
                rom_bank |= std::max(value & 0x1F, 1);
                break;
            case 0x4:
            case 0x5:
                if (mode) {
                    eram_bank = (value & 0x3) + 1;
                } else {
                    rom_bank &= 0x1F;
                    rom_bank |= (value & 0x3) << 5;
                }
                break;
            case 0x6:
            case 0x7:
                mode = value & 1;
                break;
            case 0x8:
            case 0x9:
                VRAM[address & 0x1FFF] = value;
                break;
            case 0xA:
            case 0xB:
                if (eram_enable) {
                    ERAM[eram_bank * 0x2000 + (address & 0x1FFF)] = value;
                }
                break;
            case 0xC:
                WRAM[address & 0xFFF] = value;
                break;
            case 0xD:
                WRAM[(wram_bank * 0x1000) + (address & 0x1FFF)] = value;
                break;
            case 0xE:
                WRAM[address & 0xFFF] = value;
                break;
            case 0xF:
                if (address < 0xFE00) {
                    WRAM[(wram_bank * 0x1000) + (address & 0x1FFF)] = value;
                } else if (address >= 0xFF80) {
                    HRAM[address & 0x7F] = value;
                } else if (address == 0xFF01) {
                    serial = static_cast<char>(value);
                } else if (address == 0xFF02 && value == 0x81) {
                    printf("%c", serial);
                }
                break;
        }
    };

    gb_func virtual waste() noexcept->void override {}
};
