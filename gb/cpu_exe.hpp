#pragma once
#include "cpu.hpp"
#include "cpu_alu.hpp"
#include "cpu_ctx.hpp"
#include "utils.hpp"
#include <utility>


#define GB_DECODE_INST(...) do {} while(false)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wundefined-inline"
#pragma clang diagnostic ignored "-Wunknown-pragmas"
struct gb::CPU::EXE {
    struct Table {
        Result(* const ops[256])(CTX ctx) noexcept;
    };

    // NOP
    template<byte_t OP> requires(utils::match(OP, "00000000"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        (void)ctx;
        return Result::OK;
    }

    // STOP
    template<byte_t OP> requires(utils::match(OP, "00010000"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        auto const imm = ctx.op_fetch8();
        (void)imm;
        return Result::STOP;
    }

    // DI
    template<byte_t OP> requires(utils::match(OP, "11110011"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        ctx.reg8_set<REG8::IME>(false);
        return Result::INTERRUPT_DISABLE;
    }

    // EI
    template<byte_t OP> requires(utils::match(OP, "11111011"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        ctx.reg8_set<REG8::IME>(true);
        return Result::INTERRUPT_ENABLE;
    }

    // LD (u16), SP
    template<byte_t OP> requires(utils::match(OP, "00001000"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        auto const address = ctx.op_fetch16();
        auto const value = ctx.reg16_get<REG16::SP>();
        ctx.mem16_set(address, value);
        return Result::OK;
    }

    // LD r16, u16
    template<byte_t OP> requires(utils::match(OP, "00rr0001"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        constexpr auto const reg = static_cast<REG16>((OP >> 4) & 0b11);
        auto const value = ctx.op_fetch16();
        ctx.reg16_set<reg>(value);
        return Result::OK;
    }

    // LD (r16), A
    template<byte_t OP> requires(utils::match(OP, "000r0010"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        constexpr auto const reg = static_cast<REG16>((OP >> 4) & 0b11);
        auto const address = ctx.reg16_get<reg>();
        auto const value = ctx.reg8_get<REG8::A>();
        ctx.mem8_set(address, value);
        return Result::OK;
    }

    // LD A, (r16)
    template<byte_t OP> requires(utils::match(OP, "000r1010"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        constexpr auto const reg = static_cast<REG16>((OP >> 4) & 0b11);
        auto const address = ctx.reg16_get<reg>();
        auto const value = ctx.mem8_get(address);
        ctx.reg8_set<REG8::A>(value);
        return Result::OK;
    }

    // LD (HL++), A
    template<byte_t OP> requires(utils::match(OP, "00100010"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        auto address = ctx.reg16_get<REG16::HL>();
        auto const value = ctx.reg8_get<REG8::A>();
        ctx.mem8_set(address, value);
        ctx.reg16_set<REG16::HL>(++address);
        return Result::OK;
    }

    // LD A, (HL++)
    template<byte_t OP> requires(utils::match(OP, "00101010"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        auto address = ctx.reg16_get<REG16::HL>();
        auto const value = ctx.mem8_get(address);
        ctx.reg8_set<REG8::A>(value);
        ctx.reg16_set<REG16::HL>(++address);
        return Result::OK;
    }

    // LD (HL--), A
    template<byte_t OP> requires(utils::match(OP, "00110010"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        auto address = ctx.reg16_get<REG16::HL>();
        auto const value = ctx.reg8_get<REG8::A>();
        ctx.mem8_set(address, value);
        ctx.reg16_set<REG16::HL>(--address);
        return Result::OK;
    }

    // LD A, (HL--)
    template<byte_t OP> requires(utils::match(OP, "00111010"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        auto address = ctx.reg16_get<REG16::HL>();
        auto const value = ctx.mem8_get(address);
        ctx.reg8_set<REG8::A>(value);
        ctx.reg16_set<REG16::HL>(--address);
        return Result::OK;
    }

    // LD r8, u8
    template<byte_t OP> requires(utils::match(OP, "00reg110"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        constexpr auto const reg = static_cast<REG8>((OP >> 3) & 0b111);
        auto const value = ctx.op_fetch8();
        ctx.reg8_set<reg>(value);
        return Result::OK;
    }

    // LD r8, r8
    template<byte_t OP> requires(utils::match(OP, "01regreg"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        constexpr auto const reg = static_cast<REG8>(OP & 0b111);
        constexpr auto const reg2 = static_cast<REG8>((OP >> 3) & 0b111);
        if constexpr (reg == REG8::HL && reg2 == REG8::HL) {
            return Result::HALT;
        } else {
            auto const value = ctx.reg8_get<reg>();
            ctx.reg8_set<reg2>(value);
            return Result::OK;
        }
    }

    // LD (0xFF00 + u8), A
    template<byte_t OP> requires(utils::match(OP, "11100000"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        auto const disp = ctx.op_fetch8();
        auto const address = static_cast<word_t>(0xFF00 + disp);
        auto const value = ctx.reg8_get<REG8::A>();
        ctx.mem8_set(address, value);
        return Result::OK;
    }

    // LD A, (0xFF00 + u8)
    template<byte_t OP> requires(utils::match(OP, "11110000"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        auto const disp = ctx.op_fetch8();
        auto const address = static_cast<word_t>(0xFF00 + disp);
        auto const value = ctx.mem8_get(address);
        ctx.reg8_set<REG8::A>(value);
        return Result::OK;
    }

    // LD (0xFF00 + C), A
    template<byte_t OP> requires(utils::match(OP, "11100010"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        auto const disp = ctx.reg8_get<REG8::C>();
        auto const address = static_cast<word_t>(0xFF00 + disp);
        auto const value = ctx.reg8_get<REG8::A>();
        ctx.mem8_set(address, value);
        return Result::OK;
    }

    // LD A, (0xFF00 + C)
    template<byte_t OP> requires(utils::match(OP, "11110010"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        auto const disp = ctx.reg8_get<REG8::C>();
        auto const address = static_cast<word_t>(0xFF00 + disp);
        auto const value = ctx.mem8_get(address);
        ctx.reg8_set<REG8::A>(value);
        return Result::OK;
    }

    // LD (u16), A
    template<byte_t OP> requires(utils::match(OP, "11101010"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        auto const address = ctx.op_fetch16();
        auto const value = ctx.reg8_get<REG8::A>();
        ctx.mem8_set(address, value);
        return Result::OK;
    }

    // LD A, (u16)
    template<byte_t OP> requires(utils::match(OP, "11111010"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        auto const address = ctx.op_fetch16();
        auto const value = ctx.mem8_get(address);
        ctx.reg8_set<REG8::A>(value);
        return Result::OK;
    }

    // LD HL, SP + i8
    template<byte_t OP> requires(utils::match(OP, "11111000"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        auto const flags = ctx.flags_get();
        auto const lhs = ctx.reg16_get<REG16::SP>();
        auto const rhs = static_cast<sbyte_t>(ctx.op_fetch8());
        auto const result = ALU::op_16_add8(flags, lhs, rhs);
        ctx.flags_set(result.flags);
        ctx.reg16_set<REG16::HL>(result.value);
        ctx.mem_waste();
        return Result::OK;
    }

    // LD SP, SP + i8
    template<byte_t OP> requires(utils::match(OP, "11101000"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        auto const flags = ctx.flags_get();
        auto const lhs = ctx.reg16_get<REG16::SP>();
        auto const rhs = static_cast<sbyte_t>(ctx.op_fetch8());
        auto const result = ALU::op_16_add8(flags, lhs, rhs);
        ctx.flags_set(result.flags);
        ctx.reg16_set<REG16::SP>(result.value);
        ctx.mem_waste();
        return Result::OK;
    }

    // LD SP, HL
    template<byte_t OP> requires(utils::match(OP, "11111001"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        auto const value = ctx.reg16_get<REG16::HL>();
        ctx.reg16_set<REG16::SP>(value);
        ctx.mem_waste();
        return Result::OK;
    }

    // POP r16
    template<byte_t OP> requires(utils::match(OP, "11rr0001"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        constexpr auto const reg = static_cast<REG16>((OP >> 4) & 0b11);
        constexpr auto const reg_fixed = reg == REG16::SP ? REG16::AF : reg;
        auto const value = ctx.stack_pop16();
        ctx.reg16_set<reg_fixed>(value);
        return Result::OK;
    }

    // PUSH r16
    template<byte_t OP> requires(utils::match(OP, "11rr0101"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        constexpr auto const reg = static_cast<REG16>((OP >> 4) & 0b11);
        constexpr auto const reg_fixed = reg == REG16::SP ? REG16::AF : reg;
        auto const value = ctx.reg16_get<reg_fixed>();
        ctx.stack_push16(value);
        return Result::OK;
    }

    // JR i8
    template<byte_t OP> requires(utils::match(OP, "00011000"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        auto const disp = static_cast<sbyte_t>(ctx.op_fetch8());
        ctx.mem_waste();
        ctx.jmp_rel(disp);
        return Result::OK;
    }

    // JR Z, i8
    template<byte_t OP> requires(utils::match(OP, "0010v000"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        constexpr auto const value = static_cast<bool>((OP >> 3) & 0b1);
        auto const disp = static_cast<sbyte_t>(ctx.op_fetch8());
        auto const flags = ctx.flags_get();
        if (flags.zero == value) {
            ctx.mem_waste();
            ctx.jmp_rel(disp);
        }
        return Result::OK;
    }

    // JR C, i8
    template<byte_t OP> requires(utils::match(OP, "0011v000"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        constexpr auto const value = static_cast<bool>((OP >> 3) & 0b1);
        auto const disp = static_cast<sbyte_t>(ctx.op_fetch8());
        auto const flags = ctx.flags_get();
        if (flags.carry == value) {
            ctx.mem_waste();
            ctx.jmp_rel(disp);
        }
        return Result::OK;
    }

    // JP u16
    template<byte_t OP> requires(utils::match(OP, "11000011"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        auto const address = ctx.op_fetch16();
        ctx.mem_waste();
        ctx.jmp_abs(address);
        return Result::OK;
    }

    // JP Z, u16
    template<byte_t OP> requires(utils::match(OP, "1100v010"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        constexpr auto const value = static_cast<bool>((OP >> 3) & 0b1);
        auto const address = ctx.op_fetch16();
        auto const flags = ctx.flags_get();
        if (flags.zero == value) {
            ctx.mem_waste();
            ctx.jmp_abs(address);
        }
        return Result::OK;
    }

    // JP C, u16
    template<byte_t OP> requires(utils::match(OP, "1101v010"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        constexpr auto const value = static_cast<bool>((OP >> 3) & 0b1);
        auto const address = ctx.op_fetch16();
        auto const flags = ctx.flags_get();
        if (flags.carry == value) {
            ctx.mem_waste();
            ctx.jmp_abs(address);
        }
        return Result::OK;
    }

    // JP HL
    template<byte_t OP> requires(utils::match(OP, "11101001"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        auto const address = ctx.reg16_get<REG16::HL>();
        ctx.jmp_abs(address);
        return Result::OK;
    }

    // RET
    template<byte_t OP> requires(utils::match(OP, "110i1001"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        constexpr auto const interupt = static_cast<bool>((OP >> 4) & 0b1);
        if constexpr (interupt) {
            ctx.reg8_set<REG8::IME>(true);
        }
        auto const address = ctx.stack_pop16();
        ctx.mem_waste();
        ctx.jmp_abs(address);
        return Result::OK;
    }

    // RET Z
    template<byte_t OP> requires(utils::match(OP, "1100v000"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        constexpr auto const value = static_cast<bool>((OP >> 3) & 0b1);
        auto const flags = ctx.flags_get();
        ctx.mem_waste();
        if (flags.zero == value) {
            auto const address = ctx.stack_pop16();
            ctx.mem_waste();
            ctx.jmp_abs(address);
        }
        return Result::OK;
    }

    // RET C
    template<byte_t OP> requires(utils::match(OP, "1101v000"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        constexpr auto const value = static_cast<bool>((OP >> 3) & 0b1);
        auto const flags = ctx.flags_get();
        ctx.mem_waste();
        if (flags.carry == value) {
            auto const address = ctx.stack_pop16();
            ctx.mem_waste();
            ctx.jmp_abs(address);
        }
        return Result::OK;
    }

    // CALL u16
    template<byte_t OP> requires(utils::match(OP, "11001101"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        auto const address = ctx.op_fetch16();
        auto const address_current = ctx.reg16_get<REG16::IP>();
        ctx.mem_waste();
        ctx.stack_push16(address_current);
        ctx.jmp_abs(address);
        return Result::OK;
    }

    // CALL Z, u16
    template<byte_t OP> requires(utils::match(OP, "1100v100"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        constexpr auto const value = static_cast<bool>((OP >> 3) & 0b1);
        auto const address = ctx.op_fetch16();
        auto const address_current = ctx.reg16_get<REG16::IP>();
        auto const flags = ctx.flags_get();
        if (flags.zero == value) {
            ctx.stack_push16(address_current);
            ctx.mem_waste();
            ctx.jmp_abs(address);
        }
        return Result::OK;
    }

    // CALL C, u16
    template<byte_t OP> requires(utils::match(OP, "1101v100"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        constexpr auto const value = static_cast<bool>((OP >> 3) & 0b1);
        auto const address = ctx.op_fetch16();
        auto const address_current = ctx.reg16_get<REG16::IP>();
        auto const flags = ctx.flags_get();
        if (flags.carry == value) {
            ctx.stack_push16(address_current);
            ctx.mem_waste();
            ctx.jmp_abs(address);
        }
        return Result::OK;
    }

    // RST
    template<byte_t OP> requires(utils::match(OP, "11rst111"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        constexpr auto const address = static_cast<byte_t>(OP & 0b111000);
        auto const address_current = ctx.reg16_get<REG16::IP>();
        ctx.stack_push16(address_current);
        ctx.mem_waste();
        ctx.jmp_abs(address);
        return Result::OK;
    }

    // INC r16
    template<byte_t OP> requires(utils::match(OP, "00rr0011"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        constexpr auto const reg = static_cast<REG16>((OP >> 4) & 0b11);
        auto value = ctx.reg16_get<reg>();
        ++value;
        ctx.reg16_set<reg>(value);
        return Result::OK;
    }

    // DEC r16
    template<byte_t OP> requires(utils::match(OP, "00rr1011"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        constexpr auto const reg = static_cast<REG16>((OP >> 4) & 0b11);
        auto value = ctx.reg16_get<reg>();
        --value;
        ctx.reg16_set<reg>(value);
        return Result::OK;
    }

    // INC r8
    template<byte_t OP> requires(utils::match(OP, "00reg100"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        constexpr auto const reg = static_cast<REG8>((OP >> 3) & 0b111);
        auto const flags = ctx.flags_get();
        auto const value = ctx.reg8_get<reg>();
        auto const result = ALU::op_8_inc(flags, value);
        ctx.flags_set(result.flags);
        ctx.reg8_set<reg>(result.value);
        return Result::OK;
    }

    // DEC r8
    template<byte_t OP> requires(utils::match(OP, "00reg101"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        constexpr auto const reg = static_cast<REG8>((OP >> 3) & 0b111);
        auto const flags = ctx.flags_get();
        auto const value = ctx.reg8_get<reg>();
        auto const result = ALU::op_8_dec(flags, value);
        ctx.flags_set(result.flags);
        ctx.reg8_set<reg>(result.value);
        return Result::OK;
    }

    // ADD HL, r16
    template<byte_t OP> requires(utils::match(OP, "00rr1001"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        constexpr auto const reg = static_cast<REG16>((OP >> 4) & 0b11);
        auto const flags = ctx.flags_get();
        auto const lhs = ctx.reg16_get<REG16::HL>();
        auto const rhs = ctx.reg16_get<reg>();
        auto const result = ALU::op_16_add16(flags, lhs, rhs);
        ctx.flags_set(result.flags);
        ctx.reg16_set<REG16::HL>(result.value);
        return Result::OK;
    }

    // ALU A, r8
    template<byte_t OP> requires(utils::match(OP, "10binreg"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        constexpr auto const reg = static_cast<REG8>(OP & 0b111);
        constexpr auto const op_bin = static_cast<ALU::OP_BIN>((OP >> 3) & 0b111);
        auto const flags = ctx.flags_get();
        auto const lhs = ctx.reg8_get<REG8::A>();
        auto const rhs = ctx.reg8_get<reg>();
        auto const result = ALU::template op_bin<op_bin>(flags, lhs, rhs);
        ctx.flags_set(result.flags);
        if constexpr (op_bin != ALU::OP_BIN::CMP) {
            ctx.reg8_set<REG8::A>(result.value);
        }
        return Result::OK;
    }

    // ALU A, u8
    template<byte_t OP> requires(utils::match(OP, "11bin110"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        constexpr auto const op_bin = static_cast<ALU::OP_BIN>((OP >> 3) & 0b111);
        auto const flags = ctx.flags_get();
        auto const lhs = ctx.reg8_get<REG8::A>();
        auto const rhs = ctx.op_fetch8();
        auto const result = ALU::template op_bin<op_bin>(flags, lhs, rhs);
        ctx.flags_set(result.flags);
        if constexpr (op_bin != ALU::OP_BIN::CMP) {
            ctx.reg8_set<REG8::A>(result.value);
        }
        return Result::OK;
    }

    // ROT
    template<byte_t OP> requires(utils::match(OP, "000xx111"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        constexpr auto const op_rot = static_cast<ALU::OP_ROT>((OP >> 3) & 0b111);
        auto const flags = ctx.flags_get();
        auto const lhs = ctx.reg8_get<REG8::A>();
        auto result = ALU::template op_rot<op_rot>(flags, lhs);
        result.flags.zero = false;
        ctx.flags_set(result.flags);
        ctx.reg8_set<REG8::A>(result.value);
        return Result::OK;
    }

    // DAA
    template<byte_t OP> requires(utils::match(OP, "00100111"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        auto const flags = ctx.flags_get();
        auto const lhs = ctx.reg8_get<REG8::A>();
        auto const result = ALU::op_8_daa(flags, lhs);
        ctx.flags_set(result.flags);
        ctx.reg8_set<REG8::A>(result.value);
        return Result::OK;
    }

    // CPL
    template<byte_t OP> requires(utils::match(OP, "00101111"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        auto const flags = ctx.flags_get();
        auto const lhs = ctx.reg8_get<REG8::A>();
        auto const result = ALU::op_8_inv(flags, lhs);
        ctx.flags_set(result.flags);
        ctx.reg8_set<REG8::A>(result.value);
        return Result::OK;
    }

    // SCF
    template<byte_t OP> requires(utils::match(OP, "00110111"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        auto flags = ctx.flags_get();
        flags.carry = true;
        flags.half = false;
        flags.subtract = false;
        ctx.flags_set(flags);
        return Result::OK;
    }

    // CCF
    template<byte_t OP> requires(utils::match(OP, "00111111"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        auto flags = ctx.flags_get();
        flags.carry = !flags.carry;
        flags.half = false;
        flags.subtract = false;
        ctx.flags_set(flags);
        return Result::OK;
    }

    // ROT r8
    template<byte_t OP> requires(utils::match(OP, "00rotreg"))
    [[nodiscard]] static constexpr Result op_prefix_bits(CTX ctx) noexcept {
        constexpr auto const reg = static_cast<REG8>(OP & 0b111);
        constexpr auto const op_rot = static_cast<ALU::OP_ROT>((OP >> 3) & 0b111);
        auto const flags = ctx.flags_get();
        auto const lhs = ctx.reg8_get<reg>();
        auto const result = ALU::template op_rot<op_rot>(flags, lhs);
        ctx.flags_set(result.flags);
        ctx.reg8_set<reg>(result.value);
        return Result::OK;
    }

    // BIT TEST I, r8
    template<byte_t OP> requires(utils::match(OP, "01ndxreg"))
    [[nodiscard]] static constexpr Result op_prefix_bits(CTX ctx) noexcept {
        constexpr auto const reg = static_cast<REG8>(OP & 0b111);
        constexpr auto const lhs = ((OP >> 3) & 0b111);
        auto const flags = ctx.flags_get();
        auto const rhs = ctx.reg8_get<reg>();
        auto const result = ALU::op_bit_test(flags, lhs, rhs);
        ctx.flags_set(result.flags);
        return Result::OK;
    }

    // BIT CLEAR I, r8
    template<byte_t OP> requires(utils::match(OP, "10ndxreg"))
    [[nodiscard]] static constexpr Result op_prefix_bits(CTX ctx) noexcept {
        constexpr auto const reg = static_cast<REG8>(OP & 0b111);
        constexpr auto const lhs = ((OP >> 3) & 0b111);
        auto const flags = ctx.flags_get();
        auto const rhs = ctx.reg8_get<reg>();
        auto const result = ALU::op_bit_clear(flags, lhs, rhs);
        ctx.reg8_set<reg>(result.value);
        return Result::OK;
    }

    // BIT SET I, r8
    template<byte_t OP> requires(utils::match(OP, "11ndxreg"))
    [[nodiscard]] static constexpr Result op_prefix_bits(CTX ctx) noexcept {
        constexpr auto const reg = static_cast<REG8>(OP & 0b111);
        constexpr auto const lhs = ((OP >> 3) & 0b111);
        auto const flags = ctx.flags_get();
        auto const rhs = ctx.reg8_get<reg>();
        auto const result = ALU::op_bit_set(flags, lhs, rhs);
        ctx.reg8_set<reg>(result.value);
        return Result::OK;
    }

    static constexpr Table const table_prefix = []<std::size_t...OP> (std::index_sequence<OP...>) consteval {
        return Table { &EXE::template op_prefix_bits<OP>... };
    } (std::make_index_sequence<256>());

    // BITS
    template<byte_t OP> requires(utils::match(OP, "11001011"))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        auto const op2 = ctx.op_fetch8();
        return table_prefix.ops[op2](ctx);
    }

    // BAD
    template<byte_t OP> requires(utils::match(OP, 0xD3, 0xDB, 0xDD, 0xE3, 0xE4, 0xEB, 0xEC, 0xED, 0xF4, 0xFC, 0xFD))
    [[nodiscard]] static constexpr Result op(CTX ctx) noexcept {
        (void)ctx; // Gameboy does not have this instruction
        return Result::BAD;
    }

    static constexpr Table const table_ops = []<std::size_t...OP> (std::index_sequence<OP...>) consteval {
        return Table { &EXE::template op<OP>... };
    } (std::make_index_sequence<256>());

    [[nodiscard]] constexpr static Result exec(CPU& cpu, BUS& bus) noexcept {
        auto ctx = CTX { cpu, bus };
        auto const op = ctx.op_fetch8();
        return table_ops.ops[op](ctx);
    }
};
#pragma clang diagnostic pop
