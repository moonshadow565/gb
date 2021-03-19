#pragma once
#include <bit>
#include "cpu.hpp"
#include "utils.hpp"

struct gb::CPU::ALU {
    enum class OP_BIN {
        ADD,
        ADC,
        SUB,
        SBC,
        AND,
        XOR,
        OR,
        CMP,
    };

    enum class OP_ROT {
        RCL,
        RCR,
        ROL,
        ROR,
        SAL,
        SAR,
        SWAP,
        SHR,
    };

    struct Result8 {
        Flags flags = {};
        byte_t value = {};
    };

    struct Result16 {
        Flags flags = {};
        word_t value = {};
    };

    /// Binary operations
    template <OP_BIN OP> requires(OP == OP_BIN::ADD)
    [[nodiscard]] static constexpr Result8 op_bin(Flags flags, byte_t lhs, byte_t rhs) noexcept {
        auto const result = lhs + rhs;
        flags.carry = (result >> 8);
        flags.half = ((result ^ lhs ^ rhs) >> 4) & 1;
        flags.subtract = false;
        flags.zero = (static_cast<byte_t>(result) == 0);
        return { flags, static_cast<byte_t>(result) };
    }

    template <OP_BIN OP> requires(OP == OP_BIN::ADC)
    [[nodiscard]] static constexpr Result8 op_bin(Flags flags, byte_t lhs, byte_t rhs) noexcept {
        auto const result = lhs + rhs + flags.carry;
        flags.carry = (result >> 8);
        flags.half = ((result ^ lhs ^ rhs) >> 4) & 1;
        flags.subtract = false;
        flags.zero = (static_cast<byte_t>(result) == 0);
        return { flags, static_cast<byte_t>(result) };
    }

    template <OP_BIN OP> requires(OP == OP_BIN::SUB)
    [[nodiscard]] static constexpr Result8 op_bin(Flags flags, byte_t lhs, byte_t rhs) noexcept {
        auto const result = lhs - rhs;
        flags.carry = (result >> 8);
        flags.half = ((result ^ lhs ^ rhs) >> 4) & 1;
        flags.subtract = true;
        flags.zero = (static_cast<byte_t>(result) == 0);
        return { flags, static_cast<byte_t>(result) };
    }

    template <OP_BIN OP> requires(OP == OP_BIN::SBC)
    [[nodiscard]] static constexpr Result8 op_bin(Flags flags, byte_t lhs, byte_t rhs) noexcept {
        auto const result = lhs - rhs - flags.carry;
        flags.carry = (result >> 8);
        flags.half = ((result ^ lhs ^ rhs) >> 4) & 1;
        flags.subtract = true;
        flags.zero = (static_cast<byte_t>(result) == 0);
        return { flags, static_cast<byte_t>(result) };
    }

    template <OP_BIN OP> requires(OP == OP_BIN::AND)
    [[nodiscard]] static constexpr Result8 op_bin(Flags flags, byte_t lhs, byte_t rhs) noexcept {
        auto const result = lhs & rhs;
        flags.carry = false;
        flags.half = true;
        flags.subtract = false;
        flags.zero = (static_cast<byte_t>(result) == 0);
        return { flags, static_cast<byte_t>(result) };
    }

    template <OP_BIN OP> requires(OP == OP_BIN::XOR)
    [[nodiscard]] static constexpr Result8 op_bin(Flags flags, byte_t lhs, byte_t rhs) noexcept {
        auto const result = lhs ^ rhs;
        flags.carry = false;
        flags.half = false;
        flags.subtract = false;
        flags.zero = (static_cast<byte_t>(result) == 0);
        return { flags, static_cast<byte_t>(result) };
    }

    template <OP_BIN OP> requires(OP == OP_BIN::OR)
    [[nodiscard]] static constexpr Result8 op_bin(Flags flags, byte_t lhs, byte_t rhs) noexcept {
        auto const result = lhs | rhs;
        flags.carry = false;
        flags.half = false;
        flags.subtract = false;
        flags.zero = (static_cast<byte_t>(result) == 0);
        return { flags, static_cast<byte_t>(result) };
    }

    template <OP_BIN OP> requires(OP == OP_BIN::CMP)
    [[nodiscard]] static constexpr Result8 op_bin(Flags flags, byte_t lhs, byte_t rhs) noexcept {
        auto const result = op_bin<OP_BIN::SUB>(flags, lhs, rhs);
        return { result.flags, lhs };
    }

    /// Rot operations
    template <OP_ROT OP> requires(OP == OP_ROT::RCL)
    [[nodiscard]] static constexpr Result8 op_rot(Flags flags, byte_t lhs) noexcept {
        auto const result = std::rotl(lhs, 1);
        flags.carry = (lhs >> 7) & 1;
        flags.half = false;
        flags.subtract = false;
        flags.zero = static_cast<byte_t>(result) == 0;
        return { flags, static_cast<byte_t>(result) };
    }

    template <OP_ROT OP> requires(OP == OP_ROT::RCR)
    [[nodiscard]] static constexpr Result8 op_rot(Flags flags, byte_t lhs) noexcept {
        auto const result = std::rotr(lhs, 1);
        flags.carry = lhs & 1;
        flags.half = false;
        flags.subtract = false;
        flags.zero = static_cast<byte_t>(result) == 0;
        return { flags, static_cast<byte_t>(result) };
    }

    template <OP_ROT OP> requires(OP == OP_ROT::ROL)
    [[nodiscard]] static constexpr Result8 op_rot(Flags flags, byte_t lhs) noexcept {
        auto const result = (lhs << 1) | flags.carry;
        flags.carry = (lhs >> 7) & 1;
        flags.half = false;
        flags.subtract = false;
        flags.zero = static_cast<byte_t>(result) == 0;
        return { flags, static_cast<byte_t>(result) };
    }

    template <OP_ROT OP> requires(OP == OP_ROT::ROR)
    [[nodiscard]] static constexpr Result8 op_rot(Flags flags, byte_t lhs) noexcept {
        auto const result = (lhs >> 1) | (flags.carry << 7);
        flags.carry = lhs & 1;
        flags.half = false;
        flags.subtract = false;
        flags.zero = static_cast<byte_t>(result) == 0;
        return { flags, static_cast<byte_t>(result) };
    }

    template <OP_ROT OP> requires(OP == OP_ROT::SAL)
    [[nodiscard]] static constexpr Result8 op_rot(Flags flags, byte_t lhs) noexcept {
        auto const result = (lhs << 1);
        flags.carry = (lhs >> 7) & 1;
        flags.half = false;
        flags.subtract = false;
        flags.zero = static_cast<byte_t>(result) == 0;
        return { flags, static_cast<byte_t>(result) };
    }

    template <OP_ROT OP> requires(OP == OP_ROT::SAR)
    [[nodiscard]] static constexpr Result8 op_rot(Flags flags, byte_t lhs) noexcept {
        auto const result = (static_cast<sbyte_t>(lhs) >> 1);
        flags.carry = lhs & 1;
        flags.half = false;
        flags.subtract = false;
        flags.zero = static_cast<byte_t>(result) == 0;
        return { flags, static_cast<byte_t>(result) };
    }

    template <OP_ROT OP> requires(OP == OP_ROT::SWAP)
    [[nodiscard]] static constexpr Result8 op_rot(Flags flags, byte_t lhs) noexcept {
        auto const result = std::rotl(lhs, 4);
        flags.carry = false;
        flags.half = false;
        flags.subtract = false;
        flags.zero = static_cast<byte_t>(result) == 0;
        return { flags, static_cast<byte_t>(result) };
    }

    template <OP_ROT OP> requires(OP == OP_ROT::SHR)
    [[nodiscard]] static constexpr Result8 op_rot(Flags flags, byte_t lhs) noexcept {
        auto const result = (lhs >> 1);
        flags.carry = lhs & 1;
        flags.half = false;
        flags.subtract = false;
        flags.zero = static_cast<byte_t>(result) == 0;
        return { flags, static_cast<byte_t>(result) };
    }

    /// Bit operations

    [[nodiscard]] static constexpr Result8 op_bit_test(Flags flags, byte_t lhs, byte_t rhs) noexcept {
        auto const result = (1 << lhs) & rhs;
        flags.half = true;
        flags.subtract = false;
        flags.zero = static_cast<byte_t>(result) == 0;
        return { flags, lhs };
    }

    [[nodiscard]] static constexpr Result8 op_bit_clear(Flags flags, byte_t lhs, byte_t rhs) noexcept {
        auto const result = ~(1 << lhs) & rhs;
        return { flags, static_cast<byte_t>(result) };
    }

    [[nodiscard]] static constexpr Result8 op_bit_set(Flags flags, byte_t lhs, byte_t rhs) noexcept {
        auto const result = (1 << lhs) | rhs;
        return { flags, static_cast<byte_t>(result) };
    }

    /// Misc 8 bit operations

    [[nodiscard]] static constexpr Result8 op_8_inc(Flags flags, byte_t lhs) noexcept {
        auto const result = lhs + 1;
        flags.half = (result & 0xF) == 0;
        flags.subtract = false;
        flags.zero = (static_cast<byte_t>(result) == 0);
        return { flags, static_cast<byte_t>(result) };
    }

    [[nodiscard]] static constexpr Result8 op_8_dec(Flags flags, byte_t lhs) noexcept {
        auto const result = lhs - 1;
        flags.half = (result & 0xF) == 0xF;
        flags.subtract = true;
        flags.zero = (static_cast<byte_t>(result) == 0);
        return { flags, static_cast<byte_t>(result) };
    }

    [[nodiscard]] static constexpr Result8 op_8_daa(Flags flags, byte_t lhs) noexcept {
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
        return { flags, static_cast<byte_t>(result) };
    }

    [[nodiscard]] static constexpr Result8 op_8_inv(Flags flags, byte_t lhs) noexcept {
        auto const result = ~lhs;
        flags.half = true;
        flags.subtract = true;
        return { flags, static_cast<byte_t>(result) };
    }

    /// Misc 16 bit operations

    [[nodiscard]] static constexpr Result16 op_16_add8(Flags flags, word_t lhs, sbyte_t rhs0) noexcept {
        auto const rhs = static_cast<word_t>(static_cast<sword_t>(rhs0));
        auto const result = lhs + rhs;
        flags.carry = ((lhs ^ rhs ^ result) >> 8) & 1;
        flags.half = ((lhs ^ rhs ^ result) >> 4) & 1;
        flags.subtract = false;
        flags.zero = false;
        return { flags, static_cast<word_t>(result) };
    }

    [[nodiscard]] static constexpr Result16 op_16_add16(Flags flags, word_t lhs, word_t rhs) noexcept {
        auto const result = lhs + rhs;
        flags.carry = ((lhs ^ rhs ^ result) >> 16) & 1;
        flags.half = ((lhs ^ rhs ^ result) >> 12) & 1;
        flags.subtract = false;
        return { flags, static_cast<word_t>(result) };
    } 
};
