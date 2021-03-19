#pragma once
#include "types.hpp"

struct gb::CPU final  {
    enum class Result {
        OK,
        BAD,
        STOP,
        HALT,
        INTERRUPT_DISABLE,
        INTERRUPT_ENABLE,
    };

    enum class REG8 {
        B,
        C,
        D,
        E,
        H,
        L,
        HL,
        A,
        IME,
    };

    enum class REG16 {
        BC,
        DE,
        HL,
        SP,
        AF,
        IP,
    };

    struct Flags final {
        bool carry = {};
        bool half = {};
        bool subtract = {};
        bool zero = {};

        constexpr Flags() noexcept = default;
        explicit constexpr Flags(byte_t value) noexcept :
            carry(value & 0b0001'0000),
            half(value & 0b0010'0000),
            subtract(value & 0b0100'0000),
            zero(value & 0b1000'0000)
        {}

        [[nodiscard]] constexpr explicit operator byte_t() const noexcept {
            auto result = 0;
            result |= carry << 4;
            result |= half << 5;
            result |= subtract << 6;
            result |= zero << 7;
            return  static_cast<byte_t>(result);
        }
        constexpr bool operator==(Flags const&) const noexcept = default;
        constexpr bool operator!=(Flags const&) const noexcept = default;
    };

    Flags reg_f = {};
    bool reg_ime = {};
    byte_t reg_a = {};
    byte_t reg_c = {};
    byte_t reg_b = {};
    byte_t reg_e = {};
    byte_t reg_d = {};
    byte_t reg_l = {};
    byte_t reg_h = {};
    word_t reg_sp = {};
    word_t reg_ip = {};

    struct ALU;
    struct CTX;
    struct EXE;

    Result exec(BUS& bus) noexcept;
    void trace(BUS& bus) const noexcept;
};
