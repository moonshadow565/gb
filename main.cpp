#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>

#include "gb/cpu.hpp"
#include "gb/mcb1.hpp"

using namespace gb;

int main() {
    auto mem = std::make_unique<CPU::MCB1>();
    auto cpu = CPU{};
    constexpr auto filename = "tests/cpu_instrs/cpu_instrs.gb";
    // constexpr auto filename = "tests/cpu_instrs/individual/11-op a,(hl).gb";

    if (auto file = std::ifstream(filename, std::ios::binary);
        !file.read(reinterpret_cast<char*>(mem->ROM.data()), std::filesystem::file_size(filename))) {
        printf("Failed to read file!");
        return 0;
    }
    cpu.reg_a = 0x1;
    cpu.reg_f = CPU::Flags::from_byte(0xB0);
    cpu.reg_b = 0x00;
    cpu.reg_c = 0x13;
    cpu.reg_d = 0x00;
    cpu.reg_e = 0xD8;
    cpu.reg_h = 0x01;
    cpu.reg_l = 0x4D;
    cpu.reg_sp = 0xFFFE;
    cpu.reg_ip = 0x100;
    for (long long c = 0; true; ++c) {
        //cpu.trace(*mem);
        auto const result = cpu.step(*mem);
        switch (result) {
            case CPU::Status::OK:
                break;
            case CPU::Status::BAD:
                printf("Bad instruction!\n");
                return 0;
            case CPU::Status::HALT:
                printf("Halt!\n");
                return 0;
            case CPU::Status::STOP:
                printf("Stop!\n");
                return 0;
        }
    }
    return 0;
}
