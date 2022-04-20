#include "cpu.hpp"

#include "cpu_exe.hpp"

using namespace gb;

auto CPU::step(BUS &bus) noexcept -> Status { return CPU::EXE::step(*this, bus); }

auto CPU::trace(BUS &bus) const noexcept -> void {
    auto address = this->reg_ip;
    auto const op0 = bus.read_byte(address++);
    auto const op1 = bus.read_byte(address++);
    auto const op2 = bus.read_byte(address++);
    auto const op3 = bus.read_byte(address++);
    fprintf(stderr,
            "A: %02X F: %02X "
            "B: %02X C: %02X "
            "D: %02X E: %02X "
            "H: %02X L: %02X "
            "SP: %04X "
            "PC: 00:%04X "
            "(%02X %02X %02X %02X)\n",
            this->reg_a,
            this->reg_f.into_byte(),
            this->reg_b,
            this->reg_c,
            this->reg_d,
            this->reg_e,
            this->reg_h,
            this->reg_l,
            this->reg_sp,
            this->reg_ip,
            op0,
            op1,
            op2,
            op3);
}
