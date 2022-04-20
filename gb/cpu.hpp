#pragma once
#include "common.hpp"

struct gb::CPU final {
    enum class Status : byte_t { OK, BAD, STOP, HALT };

    enum class REG8 : byte_t { B, C, D, E, H, L, HL, A };

    enum class REG16 : byte_t { BC, DE, HL, SP, AF, IP };

    struct [[nodiscard]] alignas(4) Flags final {
        bool carry = {};
        bool half = {};
        bool subtract = {};
        bool zero = {};
        gb_func static from_byte(byte_t value) noexcept->Flags {
            auto result = Flags{};
            result.carry = (value & 0b0001'0000) != 0;
            result.half = (value & 0b0010'0000) != 0;
            result.subtract = (value & 0b0100'0000) != 0;
            result.zero = (value & 0b1000'0000) != 0;
            return result;
        }
        gb_func inline into_byte() const noexcept->byte_t {
            auto result = 0;
            result |= carry << 4;
            result |= half << 5;
            result |= subtract << 6;
            result |= zero << 7;
            return static_cast<byte_t>(result);
        }
        constexpr bool operator==(Flags const&) const noexcept = default;
    };

    byte_t reg_b = {};
    byte_t reg_c = {};
    byte_t reg_d = {};
    byte_t reg_e = {};
    byte_t reg_h = {};
    byte_t reg_l = {};
    byte_t reg_a = {};
    bool reg_ime = {};
    Flags reg_f = {};
    word_t reg_sp = {};
    word_t reg_ip = {};

    struct ALU;
    struct BUS;
    struct CTX;
    struct EXE;
    struct MCB1;
    struct MCB2;
    struct MCB3;

    auto step(BUS& bus) noexcept -> Status;
    auto trace(BUS& bus) const noexcept -> void;
};
