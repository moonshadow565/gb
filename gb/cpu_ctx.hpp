#pragma once
#include "cpu.hpp"
#include "cpu_bus.hpp"

struct gb::CPU::CTX final {
    CPU& cpu;
    BUS& bus;

    /// Wasting 1 memory cycle
    gb_func inline mem_waste() noexcept->void { bus.waste(); }

    /// 8bit memory getter and setter
    gb_func inline mem8_get(word_t address) noexcept->byte_t { return bus.read_byte(address); }

    gb_func inline mem8_set(word_t address, byte_t value) noexcept->void { bus.write_byte(address, value); }

    gb_func inline mem16_get(word_t address) noexcept->word_t {
        auto const lo = mem8_get(address);
        auto const hi = mem8_get(++address);
        return word_pack(lo, hi);
    }

    gb_func inline mem16_set(word_t address, word_t value) noexcept->void {
        auto const [lo, hi] = word_unpack(value);
        mem8_set(address, lo);
        mem8_set(++address, hi);
    }

    /// Fetching opcodes

    gb_func inline op_fetch8() noexcept->byte_t { return mem8_get(cpu.reg_ip++); }

    gb_func inline op_fetch16() noexcept->word_t {
        auto const lo = mem8_get(cpu.reg_ip++);
        auto const hi = mem8_get(cpu.reg_ip++);
        return word_pack(lo, hi);
    }

    /// Flags getter and setter
    gb_func inline flags_get() noexcept->Flags { return cpu.reg_f; }

    gb_func inline flags_set(Flags flags) noexcept->void { cpu.reg_f = flags; }

    gb_func inline ime_get() noexcept->byte_t { return cpu.reg_ime; }

    gb_func inline ime_set(byte_t value) noexcept->void { cpu.reg_ime = value; }

    /// 16bit register getters

    template <REG16 R>
        requires(R == REG16::BC)
    gb_func inline reg16_get() noexcept->word_t { return word_pack(cpu.reg_c, cpu.reg_b); }

    template <REG16 R>
        requires(R == REG16::DE)
    gb_func inline reg16_get() noexcept->word_t { return word_pack(cpu.reg_e, cpu.reg_d); }

    template <REG16 R>
        requires(R == REG16::HL)
    gb_func inline reg16_get() noexcept->word_t { return word_pack(cpu.reg_l, cpu.reg_h); }

    template <REG16 R>
        requires(R == REG16::SP)
    gb_func inline reg16_get() noexcept->word_t { return cpu.reg_sp; }

    template <REG16 R>
        requires(R == REG16::AF)
    gb_func inline reg16_get() noexcept->word_t { return word_pack(cpu.reg_f.into_byte(), cpu.reg_a); }

    template <REG16 R>
        requires(R == REG16::IP)
    gb_func inline reg16_get() noexcept->word_t { return cpu.reg_ip; }

    /// 16bit register setters

    template <REG16 R>
        requires(R == REG16::BC)
    gb_func inline reg16_set(word_t value) noexcept->void {
        auto const [lo, hi] = word_unpack(value);
        cpu.reg_b = hi;
        cpu.reg_c = lo;
    }

    template <REG16 R>
        requires(R == REG16::DE)
    gb_func inline reg16_set(word_t value) noexcept->void {
        auto const [lo, hi] = word_unpack(value);
        cpu.reg_d = hi;
        cpu.reg_e = lo;
    }

    template <REG16 R>
        requires(R == REG16::HL)
    gb_func inline reg16_set(word_t value) noexcept->void {
        auto const [lo, hi] = word_unpack(value);
        cpu.reg_h = hi;
        cpu.reg_l = lo;
    }

    template <REG16 R>
        requires(R == REG16::SP)
    gb_func inline reg16_set(word_t value) noexcept->void { cpu.reg_sp = value; }

    template <REG16 R>
        requires(R == REG16::AF)
    gb_func inline reg16_set(word_t value) noexcept->void {
        auto const [lo, hi] = word_unpack(value);
        cpu.reg_a = hi;
        cpu.reg_f = Flags::from_byte(lo);
    }

    template <REG16 R>
        requires(R == REG16::IP)
    gb_func inline reg16_set(word_t value) noexcept->void { cpu.reg_ip = value; }

    /// 8 bit getters

    template <REG8 R>
        requires(R == REG8::B)
    gb_func inline reg8_get() noexcept->byte_t { return cpu.reg_b; }

    template <REG8 R>
        requires(R == REG8::C)
    gb_func inline reg8_get() noexcept->byte_t { return cpu.reg_c; }

    template <REG8 R>
        requires(R == REG8::D)
    gb_func inline reg8_get() noexcept->byte_t { return cpu.reg_d; }

    template <REG8 R>
        requires(R == REG8::E)
    gb_func inline reg8_get() noexcept->byte_t { return cpu.reg_e; }

    template <REG8 R>
        requires(R == REG8::H)
    gb_func inline reg8_get() noexcept->byte_t { return cpu.reg_h; }

    template <REG8 R>
        requires(R == REG8::L)
    gb_func inline reg8_get() noexcept->byte_t { return cpu.reg_l; }

    template <REG8 R>
        requires(R == REG8::HL)
    gb_func inline reg8_get() noexcept->byte_t {
        auto const address = reg16_get<REG16::HL>();
        return mem8_get(address);
    }

    template <REG8 R>
        requires(R == REG8::A)
    gb_func inline reg8_get() noexcept->byte_t { return cpu.reg_a; }

    /// 8 bit setters

    template <REG8 R>
        requires(R == REG8::B)
    gb_func inline reg8_set(byte_t value) noexcept->void { cpu.reg_b = value; }

    template <REG8 R>
        requires(R == REG8::C)
    gb_func inline reg8_set(byte_t value) noexcept->void { cpu.reg_c = value; }

    template <REG8 R>
        requires(R == REG8::D)
    gb_func inline reg8_set(byte_t value) noexcept->void { cpu.reg_d = value; }

    template <REG8 R>
        requires(R == REG8::E)
    gb_func inline reg8_set(byte_t value) noexcept->void { cpu.reg_e = value; }

    template <REG8 R>
        requires(R == REG8::H)
    gb_func inline reg8_set(byte_t value) noexcept->void { cpu.reg_h = value; }

    template <REG8 R>
        requires(R == REG8::L)
    gb_func inline reg8_set(byte_t value) noexcept->void { cpu.reg_l = value; }

    template <REG8 R>
        requires(R == REG8::HL)
    gb_func inline reg8_set(byte_t value) noexcept->void {
        auto const address = reg16_get<REG16::HL>();
        mem8_set(address, value);
    }

    template <REG8 R>
        requires(R == REG8::A)
    gb_func inline reg8_set(byte_t value) noexcept->void { cpu.reg_a = value; }

    /// Jumps

    gb_func inline jmp_rel(sbyte_t disp) noexcept->void { cpu.reg_ip += disp; }

    gb_func inline jmp_abs(word_t address) noexcept->void { cpu.reg_ip = address; }

    /// 16bit stack
    gb_func inline stack_pop16() noexcept->word_t {
        auto const value = mem16_get(cpu.reg_sp);
        cpu.reg_sp += 2;
        return value;
    }

    gb_func inline stack_push16(word_t value) noexcept->void {
        cpu.reg_sp -= 2;
        mem16_set(cpu.reg_sp, value);
    }
};
