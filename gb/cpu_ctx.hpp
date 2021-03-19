#pragma once
#include "bus.hpp"
#include "cpu.hpp"
#include "utils.hpp"

struct gb::CPU::CTX final {
    CPU& cpu;
    BUS& bus;

    /// Wasting 1 memory cycle
    constexpr void mem_waste() noexcept {
        bus.waste();
    }

    /// 8bit memory getter and setter
    [[nodiscard]] constexpr byte_t mem8_get(word_t address) noexcept {
        return bus.read_byte(address);
    }

    constexpr void mem8_set(word_t address, byte_t value) noexcept {
        bus.write_byte(address, value);
    }

    [[nodiscard]] constexpr word_t mem16_get(word_t address) noexcept {
        auto const lo = mem8_get(address);
        auto const hi = mem8_get(++address);
        return utils::word_pack(lo, hi);
    }

    constexpr void mem16_set(word_t address, word_t value) noexcept {
        auto const [lo, hi] = utils::word_unpack(value);
        mem8_set(address, lo);
        mem8_set(++address, hi);
    }

    /// Fetching opcodes

    [[nodiscard]] constexpr byte_t op_fetch8() noexcept {
        return mem8_get(cpu.reg_ip++);
    }

    [[nodiscard]] constexpr word_t op_fetch16() noexcept {
        auto const lo = mem8_get(cpu.reg_ip++);
        auto const hi = mem8_get(cpu.reg_ip++);
        return utils::word_pack(lo, hi);
    }

    /// Flags getter and setter
    [[nodiscard]] constexpr Flags flags_get() noexcept {
        return cpu.reg_f;
    }

    constexpr void flags_set(Flags flags) noexcept {
        cpu.reg_f = flags;
    }

    /// 16bit register getters

    template <REG16 R> requires(R == REG16::BC)
    [[nodiscard]] constexpr word_t reg16_get() noexcept {
        return utils::word_pack(cpu.reg_c, cpu.reg_b);
    }

    template <REG16 R> requires(R == REG16::DE)
    [[nodiscard]] constexpr word_t reg16_get() noexcept {
        return utils::word_pack(cpu.reg_e, cpu.reg_d);
    }

    template <REG16 R> requires(R == REG16::HL)
    [[nodiscard]] constexpr word_t reg16_get() noexcept {
        return utils::word_pack(cpu.reg_l, cpu.reg_h);
    }

    template <REG16 R> requires(R == REG16::SP)
    [[nodiscard]] constexpr word_t reg16_get() noexcept {
        return cpu.reg_sp;
    }

    template <REG16 R> requires(R == REG16::AF)
    [[nodiscard]] constexpr word_t reg16_get() noexcept {
        return utils::word_pack(static_cast<byte_t>(cpu.reg_f), cpu.reg_a);
    }

    template <REG16 R> requires(R == REG16::IP)
    [[nodiscard]] constexpr word_t reg16_get() noexcept {
        return cpu.reg_ip;
    }

    /// 16bit register setters

    template <REG16 R> requires(R == REG16::BC)
    constexpr void reg16_set(word_t value) noexcept {
        auto const [lo, hi] = utils::word_unpack(value);
        cpu.reg_c = lo;
        cpu.reg_b = hi;
    }

    template <REG16 R> requires(R == REG16::DE)
    constexpr void reg16_set(word_t value) noexcept {
        auto const [lo, hi] = utils::word_unpack(value);
        cpu.reg_e = lo;
        cpu.reg_d = hi;
    }

    template <REG16 R> requires(R == REG16::HL)
    constexpr void reg16_set(word_t value) noexcept {
        auto const [lo, hi] = utils::word_unpack(value);
        cpu.reg_l = lo;
        cpu.reg_h = hi;
    }

    template <REG16 R> requires(R == REG16::SP)
    constexpr void reg16_set(word_t value) noexcept {
        cpu.reg_sp = value;
    }

    template <REG16 R> requires(R == REG16::AF)
    constexpr void reg16_set(word_t value) noexcept {
        auto const [lo, hi] = utils::word_unpack(value);
        cpu.reg_f = static_cast<Flags>(lo);
        cpu.reg_a = hi;
    }

    template <REG16 R> requires(R == REG16::IP)
    constexpr void reg16_set(word_t value) noexcept {
        cpu.reg_ip = value;
    }

    /// 8 bit getters

    template <REG8 R> requires(R == REG8::B)
    [[nodiscard]] constexpr byte_t reg8_get() noexcept {
        return cpu.reg_b;
    }

    template <REG8 R> requires(R == REG8::C)
    [[nodiscard]] constexpr byte_t reg8_get() noexcept {
        return cpu.reg_c;
    }

    template <REG8 R> requires(R == REG8::D)
    [[nodiscard]] constexpr byte_t reg8_get() noexcept {
        return cpu.reg_d;
    }

    template <REG8 R> requires(R == REG8::E)
    [[nodiscard]] constexpr byte_t reg8_get() noexcept {
        return cpu.reg_e;
    }

    template <REG8 R> requires(R == REG8::H)
    [[nodiscard]] constexpr byte_t reg8_get() noexcept {
        return cpu.reg_h;
    }

    template <REG8 R> requires(R == REG8::L)
    [[nodiscard]] constexpr byte_t reg8_get() noexcept {
        return cpu.reg_l;
    }

    template <REG8 R> requires(R == REG8::HL)
    [[nodiscard]] constexpr byte_t reg8_get() noexcept {
        auto const address = reg16_get<REG16::HL>();
        return mem8_get(address);
    }

    template <REG8 R> requires(R == REG8::A)
    [[nodiscard]] constexpr byte_t reg8_get() noexcept {
        return cpu.reg_a;
    }

    template <REG8 R> requires(R == REG8::IME)
    [[nodiscard]] constexpr byte_t reg8_get() noexcept {
        return cpu.reg_ime;
    }

    /// 8 bit setters

    template <REG8 R> requires(R == REG8::B)
    constexpr void reg8_set(byte_t value) noexcept {
        cpu.reg_b = value;
    }

    template <REG8 R> requires(R == REG8::C)
    constexpr void reg8_set(byte_t value) noexcept {
        cpu.reg_c = value;
    }

    template <REG8 R> requires(R == REG8::D)
    constexpr void reg8_set(byte_t value) noexcept {
        cpu.reg_d = value;
    }

    template <REG8 R> requires(R == REG8::E)
    constexpr void reg8_set(byte_t value) noexcept {
        cpu.reg_e = value;
    }

    template <REG8 R> requires(R == REG8::H)
    constexpr void reg8_set(byte_t value) noexcept {
        cpu.reg_h = value;
    }

    template <REG8 R> requires(R == REG8::L)
    constexpr void reg8_set(byte_t value) noexcept {
        cpu.reg_l = value;
    }

    template <REG8 R> requires(R == REG8::HL)
    constexpr void reg8_set(byte_t value) noexcept {
        auto const address = reg16_get<REG16::HL>();
        mem8_set(address, value);
    }

    template <REG8 R> requires(R == REG8::A)
    constexpr void reg8_set(byte_t value) noexcept {
        cpu.reg_a = value;
    }

    template <REG8 R> requires(R == REG8::IME)
    constexpr void reg8_set(byte_t value) noexcept {
        cpu.reg_ime = value;
    }

    /// Jumps
    constexpr void jmp_rel(sbyte_t disp) noexcept {
        cpu.reg_ip += disp;
    }

    constexpr void jmp_abs(word_t address) noexcept {
        cpu.reg_ip = address;
    }

    /// 16bit stack
    [[nodiscard]] constexpr word_t stack_pop16() noexcept {
        auto const value = mem16_get(cpu.reg_sp);
        cpu.reg_sp += 2;
        return value;
    }

    constexpr void stack_push16(word_t value) noexcept {
        cpu.reg_sp -= 2;
        mem16_set(cpu.reg_sp, value);
    }
};
