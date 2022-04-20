#pragma once
#include <bit>

#include "cpu.hpp"

struct gb::CPU::ALU {
    enum class OP_BIN { ADD, ADC, SUB, SBC, AND, XOR, OR, CMP };

    enum class OP_BIT { ROT, TEST, CLEAR, SET };

    enum class OP_ROT { RCL, RCR, ROL, ROR, SAL, SAR, SWAP, SHR };

    struct [[nodiscard]] Result8 {
        Flags flags = {};
        byte_t value = {};
    };

    struct [[nodiscard]] Result16 {
        Flags flags = {};
        word_t value = {};
    };

    /// Binary operations
    template <OP_BIN OP>
        requires(OP == OP_BIN::ADD)
    gb_func static op_bin(Flags flags, byte_t lhs, byte_t rhs) noexcept->Result8 {
        auto const result = lhs + rhs;
        flags.carry = (result >> 8);
        flags.half = ((result ^ lhs ^ rhs) >> 4) & 1;
        flags.subtract = false;
        flags.zero = (static_cast<byte_t>(result) == 0);
        return {flags, static_cast<byte_t>(result)};
    }

    template <OP_BIN OP>
        requires(OP == OP_BIN::ADC)
    gb_func static op_bin(Flags flags, byte_t lhs, byte_t rhs) noexcept->Result8 {
        auto const result = lhs + rhs + flags.carry;
        flags.carry = (result >> 8);
        flags.half = ((result ^ lhs ^ rhs) >> 4) & 1;
        flags.subtract = false;
        flags.zero = (static_cast<byte_t>(result) == 0);
        return {flags, static_cast<byte_t>(result)};
    }

    template <OP_BIN OP>
        requires(OP == OP_BIN::SUB)
    gb_func static op_bin(Flags flags, byte_t lhs, byte_t rhs) noexcept->Result8 {
        auto const result = lhs - rhs;
        flags.carry = (result >> 8);
        flags.half = ((result ^ lhs ^ rhs) >> 4) & 1;
        flags.subtract = true;
        flags.zero = (static_cast<byte_t>(result) == 0);
        return {flags, static_cast<byte_t>(result)};
    }

    template <OP_BIN OP>
        requires(OP == OP_BIN::SBC)
    gb_func static op_bin(Flags flags, byte_t lhs, byte_t rhs) noexcept->Result8 {
        auto const result = lhs - rhs - flags.carry;
        flags.carry = (result >> 8);
        flags.half = ((result ^ lhs ^ rhs) >> 4) & 1;
        flags.subtract = true;
        flags.zero = (static_cast<byte_t>(result) == 0);
        return {flags, static_cast<byte_t>(result)};
    }

    template <OP_BIN OP>
        requires(OP == OP_BIN::AND)
    gb_func static op_bin(Flags flags, byte_t lhs, byte_t rhs) noexcept->Result8 {
        auto const result = lhs & rhs;
        flags.carry = false;
        flags.half = true;
        flags.subtract = false;
        flags.zero = (static_cast<byte_t>(result) == 0);
        return {flags, static_cast<byte_t>(result)};
    }

    template <OP_BIN OP>
        requires(OP == OP_BIN::XOR)
    gb_func static op_bin(Flags flags, byte_t lhs, byte_t rhs) noexcept->Result8 {
        auto const result = lhs ^ rhs;
        flags.carry = false;
        flags.half = false;
        flags.subtract = false;
        flags.zero = (static_cast<byte_t>(result) == 0);
        return {flags, static_cast<byte_t>(result)};
    }

    template <OP_BIN OP>
        requires(OP == OP_BIN::OR)
    gb_func static op_bin(Flags flags, byte_t lhs, byte_t rhs) noexcept->Result8 {
        auto const result = lhs | rhs;
        flags.carry = false;
        flags.half = false;
        flags.subtract = false;
        flags.zero = (static_cast<byte_t>(result) == 0);
        return {flags, static_cast<byte_t>(result)};
    }

    template <OP_BIN OP>
        requires(OP == OP_BIN::CMP)
    gb_func static op_bin(Flags flags, byte_t lhs, byte_t rhs) noexcept->Result8 {
        auto const result = op_bin<OP_BIN::SUB>(flags, lhs, rhs);
        return {result.flags, lhs};
    }

    /// Rot operations
    template <OP_ROT OP>
        requires(OP == OP_ROT::RCL)
    gb_func static op_bit_rot(Flags flags, byte_t lhs) noexcept->Result8 {
        auto const result = std::rotl(lhs, 1);
        flags.carry = (lhs >> 7) & 1;
        flags.half = false;
        flags.subtract = false;
        flags.zero = static_cast<byte_t>(result) == 0;
        return {flags, static_cast<byte_t>(result)};
    }

    template <OP_ROT OP>
        requires(OP == OP_ROT::RCR)
    gb_func static op_bit_rot(Flags flags, byte_t lhs) noexcept->Result8 {
        auto const result = std::rotr(lhs, 1);
        flags.carry = lhs & 1;
        flags.half = false;
        flags.subtract = false;
        flags.zero = static_cast<byte_t>(result) == 0;
        return {flags, static_cast<byte_t>(result)};
    }

    template <OP_ROT OP>
        requires(OP == OP_ROT::ROL)
    gb_func static op_bit_rot(Flags flags, byte_t lhs) noexcept->Result8 {
        auto const result = (lhs << 1) | (byte_t)flags.carry;
        flags.carry = (lhs >> 7) & 1;
        flags.half = false;
        flags.subtract = false;
        flags.zero = static_cast<byte_t>(result) == 0;
        return {flags, static_cast<byte_t>(result)};
    }

    template <OP_ROT OP>
        requires(OP == OP_ROT::ROR)
    gb_func static op_bit_rot(Flags flags, byte_t lhs) noexcept->Result8 {
        auto const result = (lhs >> 1) | (flags.carry << 7);
        flags.carry = lhs & 1;
        flags.half = false;
        flags.subtract = false;
        flags.zero = static_cast<byte_t>(result) == 0;
        return {flags, static_cast<byte_t>(result)};
    }

    template <OP_ROT OP>
        requires(OP == OP_ROT::SAL)
    gb_func static op_bit_rot(Flags flags, byte_t lhs) noexcept->Result8 {
        auto const result = (lhs << 1);
        flags.carry = (lhs >> 7) & 1;
        flags.half = false;
        flags.subtract = false;
        flags.zero = static_cast<byte_t>(result) == 0;
        return {flags, static_cast<byte_t>(result)};
    }

    template <OP_ROT OP>
        requires(OP == OP_ROT::SAR)
    gb_func static op_bit_rot(Flags flags, byte_t lhs) noexcept->Result8 {
        auto const result = (static_cast<sbyte_t>(lhs) >> 1);
        flags.carry = lhs & 1;
        flags.half = false;
        flags.subtract = false;
        flags.zero = static_cast<byte_t>(result) == 0;
        return {flags, static_cast<byte_t>(result)};
    }

    template <OP_ROT OP>
        requires(OP == OP_ROT::SWAP)
    gb_func static op_bit_rot(Flags flags, byte_t lhs) noexcept->Result8 {
        auto const result = std::rotl(lhs, 4);
        flags.carry = false;
        flags.half = false;
        flags.subtract = false;
        flags.zero = static_cast<byte_t>(result) == 0;
        return {flags, static_cast<byte_t>(result)};
    }

    template <OP_ROT OP>
        requires(OP == OP_ROT::SHR)
    gb_func static op_bit_rot(Flags flags, byte_t lhs) noexcept->Result8 {
        auto const result = (lhs >> 1);
        flags.carry = lhs & 1;
        flags.half = false;
        flags.subtract = false;
        flags.zero = static_cast<byte_t>(result) == 0;
        return {flags, static_cast<byte_t>(result)};
    }

    /// Bit operations
    template <OP_BIT OP, byte_t I>
        requires(OP == OP_BIT::ROT)
    gb_func static op_bit(Flags flags, byte_t lhs) noexcept->Result8 {
        constexpr auto const ROT = static_cast<OP_ROT>(I);
        return op_bit_rot<ROT>(flags, lhs);
    }

    template <OP_BIT OP, byte_t I>
        requires(OP == OP_BIT::TEST)
    gb_func static op_bit(Flags flags, byte_t lhs) noexcept->Result8 {
        auto const result = lhs & (1 << I);
        flags.half = true;
        flags.subtract = false;
        flags.zero = static_cast<byte_t>(result) == 0;
        return {flags, lhs};
    }

    template <OP_BIT OP, byte_t I>
        requires(OP == OP_BIT::CLEAR)
    gb_func static op_bit(Flags flags, byte_t lhs) noexcept->Result8 {
        auto const result = lhs & ~(1 << I);
        return {flags, static_cast<byte_t>(result)};
    }

    template <OP_BIT OP, byte_t I>
        requires(OP == OP_BIT::SET)
    gb_func static op_bit(Flags flags, byte_t lhs) noexcept->Result8 {
        auto const result = lhs | (1 << I);
        return {flags, static_cast<byte_t>(result)};
    }

    /// Misc 8 bit operations

    gb_func static op_misc_inc(Flags flags, byte_t lhs) noexcept->Result8 {
        auto const result = lhs + 1;
        flags.half = (result & 0xF) == 0;
        flags.subtract = false;
        flags.zero = (static_cast<byte_t>(result) == 0);
        return {flags, static_cast<byte_t>(result)};
    }

    gb_func static op_misc_dec(Flags flags, byte_t lhs) noexcept->Result8 {
        auto const result = lhs - 1;
        flags.half = (result & 0xF) == 0xF;
        flags.subtract = true;
        flags.zero = (static_cast<byte_t>(result) == 0);
        return {flags, static_cast<byte_t>(result)};
    }

    gb_func static op_misc_daa(Flags flags, byte_t lhs) noexcept->Result8 {
        auto result = lhs;
        if (flags.subtract) {
            if (flags.half) {
                result -= 0x6;
            }
            if (flags.carry) {
                result -= 0x60;
                flags.carry = true;
            }
        } else {
            if (flags.half || ((lhs & 0xf) > 9)) {
                result += 0x6;
            }
            if (flags.carry || (lhs > 0x99)) {
                result += 0x60;
                flags.carry = true;
            }
        }
        result &= 0xff;
        flags.half = false;
        flags.zero = static_cast<byte_t>(result) == 0;
        return {flags, static_cast<byte_t>(result)};
    }

    gb_func static op_misc_inv(Flags flags, byte_t lhs) noexcept->Result8 {
        auto const result = ~lhs;
        flags.half = true;
        flags.subtract = true;
        return {flags, static_cast<byte_t>(result)};
    }

    /// Misc 16 bit operations

    gb_func static op_misc_add8(Flags flags, word_t lhs, sbyte_t rhs0) noexcept->Result16 {
        auto const rhs = static_cast<word_t>(static_cast<sword_t>(rhs0));
        auto const result = lhs + rhs;
        flags.carry = ((lhs ^ rhs ^ result) >> 8) & 1;
        flags.half = ((lhs ^ rhs ^ result) >> 4) & 1;
        flags.subtract = false;
        flags.zero = false;
        return {flags, static_cast<word_t>(result)};
    }

    gb_func static op_misc_add16(Flags flags, word_t lhs, word_t rhs) noexcept->Result16 {
        auto const result = lhs + rhs;
        flags.carry = ((lhs ^ rhs ^ result) >> 16) & 1;
        flags.half = ((lhs ^ rhs ^ result) >> 12) & 1;
        flags.subtract = false;
        return {flags, static_cast<word_t>(result)};
    }
};
