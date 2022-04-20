#pragma once

#include "cpu.hpp"
#include "cpu_alu.hpp"
#include "cpu_ctx.hpp"

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wundefined-inline"
#    pragma clang diagnostic ignored "-Wunknown-pragmas"
#endif  // __clang__

struct gb::CPU::EXE {
    // BAD
    template <byte_t OP>
        requires(one_of(OP, 0xD3, 0xDB, 0xDD, 0xE3, 0xE4, 0xEB, 0xEC, 0xED, 0xF4, 0xFC, 0xFD))
    gb_func static op1(CTX ctx) noexcept->Status {
        (void)ctx;  // Gameboy does not have this instruction
        return Status::BAD;
    }

    // NOP
    template <byte_t OP>
        requires(bit_match(OP, "00000000"))
    gb_func static op1(CTX ctx) noexcept->Status {
        (void)ctx;
        return Status::OK;
    }

    // STOP
    template <byte_t OP>
        requires(bit_match(OP, "00010000"))
    gb_func static op1(CTX ctx) noexcept->Status {
        auto const imm = ctx.op_fetch8();
        (void)imm;
        return Status::STOP;
    }

    // DI
    template <byte_t OP>
        requires(bit_match(OP, "11110011"))
    gb_func static op1(CTX ctx) noexcept->Status {
        ctx.ime_set(false);
        return Status::OK;
    }

    // EI
    template <byte_t OP>
        requires(bit_match(OP, "11111011"))
    gb_func static op1(CTX ctx) noexcept->Status {
        ctx.ime_set(true);
        return Status::OK;
    }

    // LD (u16), SP
    template <byte_t OP>
        requires(bit_match(OP, "00001000"))
    gb_func static op1(CTX ctx) noexcept->Status {
        auto const address = ctx.op_fetch16();
        auto const value = ctx.reg16_get<REG16::SP>();
        ctx.mem16_set(address, value);
        return Status::OK;
    }

    // LD r16, u16
    template <byte_t OP>
        requires(bit_match(OP, "00rr0001"))
    gb_func static op1(CTX ctx) noexcept->Status {
        constexpr auto const reg = static_cast<REG16>((OP >> 4) & 0b11);
        auto const value = ctx.op_fetch16();
        ctx.reg16_set<reg>(value);
        return Status::OK;
    }

    // LD (r16), A
    template <byte_t OP>
        requires(bit_match(OP, "000r0010"))
    gb_func static op1(CTX ctx) noexcept->Status {
        constexpr auto const reg = static_cast<REG16>((OP >> 4) & 0b11);
        auto const address = ctx.reg16_get<reg>();
        auto const value = ctx.reg8_get<REG8::A>();
        ctx.mem8_set(address, value);
        return Status::OK;
    }

    // LD A, (r16)
    template <byte_t OP>
        requires(bit_match(OP, "000r1010"))
    gb_func static op1(CTX ctx) noexcept->Status {
        constexpr auto const reg = static_cast<REG16>((OP >> 4) & 0b11);
        auto const address = ctx.reg16_get<reg>();
        auto const value = ctx.mem8_get(address);
        ctx.reg8_set<REG8::A>(value);
        return Status::OK;
    }

    // LD (HL++), A
    template <byte_t OP>
        requires(bit_match(OP, "00100010"))
    gb_func static op1(CTX ctx) noexcept->Status {
        auto address = ctx.reg16_get<REG16::HL>();
        auto const value = ctx.reg8_get<REG8::A>();
        ctx.mem8_set(address, value);
        ++address;
        ctx.reg16_set<REG16::HL>(address);
        return Status::OK;
    }

    // LD A, (HL++)
    template <byte_t OP>
        requires(bit_match(OP, "00101010"))
    gb_func static op1(CTX ctx) noexcept->Status {
        auto address = ctx.reg16_get<REG16::HL>();
        auto const value = ctx.mem8_get(address);
        ctx.reg8_set<REG8::A>(value);
        ++address;
        ctx.reg16_set<REG16::HL>(address);
        return Status::OK;
    }

    // LD (HL--), A
    template <byte_t OP>
        requires(bit_match(OP, "00110010"))
    gb_func static op1(CTX ctx) noexcept->Status {
        auto address = ctx.reg16_get<REG16::HL>();
        auto const value = ctx.reg8_get<REG8::A>();
        ctx.mem8_set(address, value);
        --address;
        ctx.reg16_set<REG16::HL>(address);
        return Status::OK;
    }

    // LD A, (HL--)
    template <byte_t OP>
        requires(bit_match(OP, "00111010"))
    gb_func static op1(CTX ctx) noexcept->Status {
        auto address = ctx.reg16_get<REG16::HL>();
        auto const value = ctx.mem8_get(address);
        ctx.reg8_set<REG8::A>(value);
        --address;
        ctx.reg16_set<REG16::HL>(address);
        return Status::OK;
    }

    // LD r8, u8
    template <byte_t OP>
        requires(bit_match(OP, "00reg110"))
    gb_func static op1(CTX ctx) noexcept->Status {
        constexpr auto const reg = static_cast<REG8>((OP >> 3) & 0b111);
        auto const value = ctx.op_fetch8();
        ctx.reg8_set<reg>(value);
        return Status::OK;
    }

    // LD r8, r8
    template <byte_t OP>
        requires(bit_match(OP, "01regreg"))
    gb_func static op1(CTX ctx) noexcept->Status {
        constexpr auto const reg = static_cast<REG8>(OP & 0b111);
        constexpr auto const reg2 = static_cast<REG8>((OP >> 3) & 0b111);
        if constexpr (reg == REG8::HL && reg2 == REG8::HL) {
            return Status::HALT;
        } else {
            auto const value = ctx.reg8_get<reg>();
            ctx.reg8_set<reg2>(value);
            return Status::OK;
        }
    }

    // LD (0xFF00 + u8), A
    template <byte_t OP>
        requires(bit_match(OP, "11100000"))
    gb_func static op1(CTX ctx) noexcept->Status {
        auto const disp = ctx.op_fetch8();
        auto const address = static_cast<word_t>(0xFF00 + disp);
        auto const value = ctx.reg8_get<REG8::A>();
        ctx.mem8_set(address, value);
        return Status::OK;
    }

    // LD A, (0xFF00 + u8)
    template <byte_t OP>
        requires(bit_match(OP, "11110000"))
    gb_func static op1(CTX ctx) noexcept->Status {
        auto const disp = ctx.op_fetch8();
        auto const address = static_cast<word_t>(0xFF00 + disp);
        auto const value = ctx.mem8_get(address);
        ctx.reg8_set<REG8::A>(value);
        return Status::OK;
    }

    // LD (0xFF00 + C), A
    template <byte_t OP>
        requires(bit_match(OP, "11100010"))
    gb_func static op1(CTX ctx) noexcept->Status {
        auto const disp = ctx.reg8_get<REG8::C>();
        auto const address = static_cast<word_t>(0xFF00 + disp);
        auto const value = ctx.reg8_get<REG8::A>();
        ctx.mem8_set(address, value);
        return Status::OK;
    }

    // LD A, (0xFF00 + C)
    template <byte_t OP>
        requires(bit_match(OP, "11110010"))
    gb_func static op1(CTX ctx) noexcept->Status {
        auto const disp = ctx.reg8_get<REG8::C>();
        auto const address = static_cast<word_t>(0xFF00 + disp);
        auto const value = ctx.mem8_get(address);
        ctx.reg8_set<REG8::A>(value);
        return Status::OK;
    }

    // LD (u16), A
    template <byte_t OP>
        requires(bit_match(OP, "11101010"))
    gb_func static op1(CTX ctx) noexcept->Status {
        auto const address = ctx.op_fetch16();
        auto const value = ctx.reg8_get<REG8::A>();
        ctx.mem8_set(address, value);
        return Status::OK;
    }

    // LD A, (u16)
    template <byte_t OP>
        requires(bit_match(OP, "11111010"))
    gb_func static op1(CTX ctx) noexcept->Status {
        auto const address = ctx.op_fetch16();
        auto const value = ctx.mem8_get(address);
        ctx.reg8_set<REG8::A>(value);
        return Status::OK;
    }

    // LD HL, SP + i8
    template <byte_t OP>
        requires(bit_match(OP, "11111000"))
    gb_func static op1(CTX ctx) noexcept->Status {
        auto const flags = ctx.flags_get();
        auto const lhs = ctx.reg16_get<REG16::SP>();
        auto const rhs = static_cast<sbyte_t>(ctx.op_fetch8());
        auto const result = ALU::op_misc_add8(flags, lhs, rhs);
        ctx.flags_set(result.flags);
        ctx.reg16_set<REG16::HL>(result.value);
        ctx.mem_waste();
        return Status::OK;
    }

    // LD SP, SP + i8
    template <byte_t OP>
        requires(bit_match(OP, "11101000"))
    gb_func static op1(CTX ctx) noexcept->Status {
        auto const flags = ctx.flags_get();
        auto const lhs = ctx.reg16_get<REG16::SP>();
        auto const rhs = static_cast<sbyte_t>(ctx.op_fetch8());
        auto const result = ALU::op_misc_add8(flags, lhs, rhs);
        ctx.flags_set(result.flags);
        ctx.reg16_set<REG16::SP>(result.value);
        ctx.mem_waste();
        return Status::OK;
    }

    // LD SP, HL
    template <byte_t OP>
        requires(bit_match(OP, "11111001"))
    gb_func static op1(CTX ctx) noexcept->Status {
        auto const value = ctx.reg16_get<REG16::HL>();
        ctx.reg16_set<REG16::SP>(value);
        ctx.mem_waste();
        return Status::OK;
    }

    // POP r16
    template <byte_t OP>
        requires(bit_match(OP, "11rr0001"))
    gb_func static op1(CTX ctx) noexcept->Status {
        constexpr auto const reg = static_cast<REG16>((OP >> 4) & 0b11);
        constexpr auto const reg_fixed = reg == REG16::SP ? REG16::AF : reg;
        auto const value = ctx.stack_pop16();
        ctx.reg16_set<reg_fixed>(value);
        return Status::OK;
    }

    // PUSH r16
    template <byte_t OP>
        requires(bit_match(OP, "11rr0101"))
    gb_func static op1(CTX ctx) noexcept->Status {
        constexpr auto const reg = static_cast<REG16>((OP >> 4) & 0b11);
        constexpr auto const reg_fixed = reg == REG16::SP ? REG16::AF : reg;
        auto const value = ctx.reg16_get<reg_fixed>();
        ctx.stack_push16(value);
        return Status::OK;
    }

    // JR i8
    template <byte_t OP>
        requires(bit_match(OP, "00011000"))
    gb_func static op1(CTX ctx) noexcept->Status {
        auto const disp = static_cast<sbyte_t>(ctx.op_fetch8());
        ctx.mem_waste();
        ctx.jmp_rel(disp);
        return Status::OK;
    }

    // JR Z, i8
    template <byte_t OP>
        requires(bit_match(OP, "0010v000"))
    gb_func static op1(CTX ctx) noexcept->Status {
        constexpr auto const value = static_cast<bool>((OP >> 3) & 0b1);
        auto const disp = static_cast<sbyte_t>(ctx.op_fetch8());
        auto const flags = ctx.flags_get();
        if (flags.zero == value) {
            ctx.mem_waste();
            ctx.jmp_rel(disp);
        }
        return Status::OK;
    }

    // JR C, i8
    template <byte_t OP>
        requires(bit_match(OP, "0011v000"))
    gb_func static op1(CTX ctx) noexcept->Status {
        constexpr auto const value = static_cast<bool>((OP >> 3) & 0b1);
        auto const disp = static_cast<sbyte_t>(ctx.op_fetch8());
        auto const flags = ctx.flags_get();
        if (flags.carry == value) {
            ctx.mem_waste();
            ctx.jmp_rel(disp);
        }
        return Status::OK;
    }

    // JP u16
    template <byte_t OP>
        requires(bit_match(OP, "11000011"))
    gb_func static op1(CTX ctx) noexcept->Status {
        auto const address = ctx.op_fetch16();
        ctx.mem_waste();
        ctx.jmp_abs(address);
        return Status::OK;
    }

    // JP Z, u16
    template <byte_t OP>
        requires(bit_match(OP, "1100v010"))
    gb_func static op1(CTX ctx) noexcept->Status {
        constexpr auto const value = static_cast<bool>((OP >> 3) & 0b1);
        auto const address = ctx.op_fetch16();
        auto const flags = ctx.flags_get();
        if (flags.zero == value) {
            ctx.mem_waste();
            ctx.jmp_abs(address);
        }
        return Status::OK;
    }

    // JP C, u16
    template <byte_t OP>
        requires(bit_match(OP, "1101v010"))
    gb_func static op1(CTX ctx) noexcept->Status {
        constexpr auto const value = static_cast<bool>((OP >> 3) & 0b1);
        auto const address = ctx.op_fetch16();
        auto const flags = ctx.flags_get();
        if (flags.carry == value) {
            ctx.mem_waste();
            ctx.jmp_abs(address);
        }
        return Status::OK;
    }

    // JP HL
    template <byte_t OP>
        requires(bit_match(OP, "11101001"))
    gb_func static op1(CTX ctx) noexcept->Status {
        auto const address = ctx.reg16_get<REG16::HL>();
        ctx.jmp_abs(address);
        return Status::OK;
    }

    // RET
    template <byte_t OP>
        requires(bit_match(OP, "110i1001"))
    gb_func static op1(CTX ctx) noexcept->Status {
        constexpr auto const interupt = static_cast<bool>((OP >> 4) & 0b1);
        if constexpr (interupt) {
            ctx.ime_set(true);
        }
        auto const address = ctx.stack_pop16();
        ctx.mem_waste();
        ctx.jmp_abs(address);
        return Status::OK;
    }

    // RET Z
    template <byte_t OP>
        requires(bit_match(OP, "1100v000"))
    gb_func static op1(CTX ctx) noexcept->Status {
        constexpr auto const value = static_cast<bool>((OP >> 3) & 0b1);
        auto const flags = ctx.flags_get();
        ctx.mem_waste();
        if (flags.zero == value) {
            auto const address = ctx.stack_pop16();
            ctx.mem_waste();
            ctx.jmp_abs(address);
        }
        return Status::OK;
    }

    // RET C
    template <byte_t OP>
        requires(bit_match(OP, "1101v000"))
    gb_func static op1(CTX ctx) noexcept->Status {
        constexpr auto const value = static_cast<bool>((OP >> 3) & 0b1);
        auto const flags = ctx.flags_get();
        ctx.mem_waste();
        if (flags.carry == value) {
            auto const address = ctx.stack_pop16();
            ctx.mem_waste();
            ctx.jmp_abs(address);
        }
        return Status::OK;
    }

    // CALL u16
    template <byte_t OP>
        requires(bit_match(OP, "11001101"))
    gb_func static op1(CTX ctx) noexcept->Status {
        auto const address = ctx.op_fetch16();
        auto const address_current = ctx.reg16_get<REG16::IP>();
        ctx.mem_waste();
        ctx.stack_push16(address_current);
        ctx.jmp_abs(address);
        return Status::OK;
    }

    // CALL Z, u16
    template <byte_t OP>
        requires(bit_match(OP, "1100v100"))
    gb_func static op1(CTX ctx) noexcept->Status {
        constexpr auto const value = static_cast<bool>((OP >> 3) & 0b1);
        auto const address = ctx.op_fetch16();
        auto const flags = ctx.flags_get();
        if (flags.zero == value) {
            auto const address_current = ctx.reg16_get<REG16::IP>();
            ctx.mem_waste();
            ctx.stack_push16(address_current);
            ctx.jmp_abs(address);
        }
        return Status::OK;
    }

    // CALL C, u16
    template <byte_t OP>
        requires(bit_match(OP, "1101v100"))
    gb_func static op1(CTX ctx) noexcept->Status {
        constexpr auto const value = static_cast<bool>((OP >> 3) & 0b1);
        auto const address = ctx.op_fetch16();
        auto const flags = ctx.flags_get();
        if (flags.carry == value) {
            auto const address_current = ctx.reg16_get<REG16::IP>();
            ctx.mem_waste();
            ctx.stack_push16(address_current);
            ctx.jmp_abs(address);
        }
        return Status::OK;
    }

    // RST
    template <byte_t OP>
        requires(bit_match(OP, "11rst111"))
    gb_func static op1(CTX ctx) noexcept->Status {
        constexpr auto const address = static_cast<byte_t>(OP & 0b111000);
        auto const address_current = ctx.reg16_get<REG16::IP>();
        ctx.mem_waste();
        ctx.stack_push16(address_current);
        ctx.jmp_abs(address);
        return Status::OK;
    }

    // INC r16
    template <byte_t OP>
        requires(bit_match(OP, "00rr0011"))
    gb_func static op1(CTX ctx) noexcept->Status {
        constexpr auto const reg = static_cast<REG16>((OP >> 4) & 0b11);
        auto value = ctx.reg16_get<reg>();
        ++value;
        ctx.reg16_set<reg>(value);
        return Status::OK;
    }

    // DEC r16
    template <byte_t OP>
        requires(bit_match(OP, "00rr1011"))
    gb_func static op1(CTX ctx) noexcept->Status {
        constexpr auto const reg = static_cast<REG16>((OP >> 4) & 0b11);
        auto value = ctx.reg16_get<reg>();
        --value;
        ctx.reg16_set<reg>(value);
        return Status::OK;
    }

    // INC r8
    template <byte_t OP>
        requires(bit_match(OP, "00reg100"))
    gb_func static op1(CTX ctx) noexcept->Status {
        constexpr auto const reg = static_cast<REG8>((OP >> 3) & 0b111);
        auto const flags = ctx.flags_get();
        auto const value = ctx.reg8_get<reg>();
        auto const result = ALU::op_misc_inc(flags, value);
        ctx.flags_set(result.flags);
        ctx.reg8_set<reg>(result.value);
        return Status::OK;
    }

    // DEC r8
    template <byte_t OP>
        requires(bit_match(OP, "00reg101"))
    gb_func static op1(CTX ctx) noexcept->Status {
        constexpr auto const reg = static_cast<REG8>((OP >> 3) & 0b111);
        auto const flags = ctx.flags_get();
        auto const value = ctx.reg8_get<reg>();
        auto const result = ALU::op_misc_dec(flags, value);
        ctx.flags_set(result.flags);
        ctx.reg8_set<reg>(result.value);
        return Status::OK;
    }

    // ADD HL, r16
    template <byte_t OP>
        requires(bit_match(OP, "00rr1001"))
    gb_func static op1(CTX ctx) noexcept->Status {
        constexpr auto const reg = static_cast<REG16>((OP >> 4) & 0b11);
        auto const flags = ctx.flags_get();
        auto const lhs = ctx.reg16_get<REG16::HL>();
        auto const rhs = ctx.reg16_get<reg>();
        auto const result = ALU::op_misc_add16(flags, lhs, rhs);
        ctx.flags_set(result.flags);
        ctx.reg16_set<REG16::HL>(result.value);
        return Status::OK;
    }

    // ALU A, r8
    template <byte_t OP>
        requires(bit_match(OP, "10binreg"))
    gb_func static op1(CTX ctx) noexcept->Status {
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
        return Status::OK;
    }

    // ALU A, u8
    template <byte_t OP>
        requires(bit_match(OP, "11bin110"))
    gb_func static op1(CTX ctx) noexcept->Status {
        constexpr auto const op_bin = static_cast<ALU::OP_BIN>((OP >> 3) & 0b111);
        auto const flags = ctx.flags_get();
        auto const lhs = ctx.reg8_get<REG8::A>();
        auto const rhs = ctx.op_fetch8();
        auto const result = ALU::template op_bin<op_bin>(flags, lhs, rhs);
        ctx.flags_set(result.flags);
        if constexpr (op_bin != ALU::OP_BIN::CMP) {
            ctx.reg8_set<REG8::A>(result.value);
        }
        return Status::OK;
    }

    // ROT
    template <byte_t OP>
        requires(bit_match(OP, "000xx111"))
    gb_func static op1(CTX ctx) noexcept->Status {
        constexpr auto const op_rot = static_cast<ALU::OP_ROT>((OP >> 3) & 0b111);
        auto const flags = ctx.flags_get();
        auto const lhs = ctx.reg8_get<REG8::A>();
        auto result = ALU::template op_bit_rot<op_rot>(flags, lhs);
        result.flags.zero = false;
        ctx.flags_set(result.flags);
        ctx.reg8_set<REG8::A>(result.value);
        return Status::OK;
    }

    // DAA
    template <byte_t OP>
        requires(bit_match(OP, "00100111"))
    gb_func static op1(CTX ctx) noexcept->Status {
        auto const flags = ctx.flags_get();
        auto const lhs = ctx.reg8_get<REG8::A>();
        auto const result = ALU::op_misc_daa(flags, lhs);
        ctx.flags_set(result.flags);
        ctx.reg8_set<REG8::A>(result.value);
        return Status::OK;
    }

    // CPL
    template <byte_t OP>
        requires(bit_match(OP, "00101111"))
    gb_func static op1(CTX ctx) noexcept->Status {
        auto const flags = ctx.flags_get();
        auto const lhs = ctx.reg8_get<REG8::A>();
        auto const result = ALU::op_misc_inv(flags, lhs);
        ctx.flags_set(result.flags);
        ctx.reg8_set<REG8::A>(result.value);
        return Status::OK;
    }

    // SCF
    template <byte_t OP>
        requires(bit_match(OP, "00110111"))
    gb_func static op1(CTX ctx) noexcept->Status {
        auto flags = ctx.flags_get();
        flags.carry = true;
        flags.half = false;
        flags.subtract = false;
        ctx.flags_set(flags);
        return Status::OK;
    }

    // CCF
    template <byte_t OP>
        requires(bit_match(OP, "00111111"))
    gb_func static op1(CTX ctx) noexcept->Status {
        auto flags = ctx.flags_get();
        flags.carry = !flags.carry;
        flags.half = false;
        flags.subtract = false;
        ctx.flags_set(flags);
        return Status::OK;
    }

    // OP_BIT I, r8
    template <byte_t OP> gb_func static op2(CTX ctx) noexcept->Status {
        constexpr auto const reg = static_cast<REG8>(OP & 0b111);
        constexpr auto const index = static_cast<byte_t>((OP >> 3) & 0b111);
        constexpr auto const op_bit = static_cast<ALU::OP_BIT>((OP >> 6) & 0b11);
        auto const flags = ctx.flags_get();
        auto const lhs = ctx.reg8_get<reg>();
        auto const result = ALU::template op_bit<op_bit, index>(flags, lhs);
        ctx.flags_set(result.flags);
        if constexpr (op_bit != ALU::OP_BIT::TEST) {
            ctx.reg8_set<reg>(result.value);
        }
        return Status::OK;
    }

    // BIT OP_BIT I, r8
    template <byte_t OP>
        requires(bit_match(OP, "11001011"))
    gb_func static op1(CTX ctx) noexcept->Status {
        auto const op2 = ctx.op_fetch8();
        return table_op2.ops[op2](ctx);
    }

    struct Table {
        Status (*const ops[256])(CTX ctx) noexcept;
    };

    static constexpr Table const table_op2 = gb_rep(256, OP, return Table{&EXE::template op2<OP>...};);

    static constexpr Table const table_op1 = gb_rep(256, OP, return Table{&EXE::template op1<OP>...};);

    gb_func static step(CPU& cpu, BUS& bus) noexcept->Status {
        auto ctx = CTX{cpu, bus};
        auto const op = ctx.op_fetch8();
        return table_op1.ops[op](ctx);
    }
};

#ifdef __clang__
#    pragma clang diagnostic pop
#endif  // __clang__
