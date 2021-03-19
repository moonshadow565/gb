#include <iostream>
#include <fstream>
#include <memory>
#include "gb/cpu.hpp"
#include "gb/mcb1.hpp"


using namespace gb;

int main() {
    auto mem = std::make_unique<MCB1>();
    auto cpu = CPU{};
    auto file = std::ifstream("tests/cpu_instrs/cpu_instrs.gb", std::ios::binary);
    if (!file.read(reinterpret_cast<char*>(mem->ROM.data()), 0x10000)) {
        printf("Failed to read file!");
        return 0;
    }
    cpu.reg_a = 0x1;
    cpu.reg_f = static_cast<CPU::Flags>(0xB0);
    cpu.reg_b = 0x00;
    cpu.reg_c = 0x13;
    cpu.reg_d = 0x00;
    cpu.reg_e = 0xD8;
    cpu.reg_h = 0x01;
    cpu.reg_l = 0x4D;
    cpu.reg_sp = 0xFFFE;
    cpu.reg_ip = 0x100;
    for(long long c = 0; true ; ++c) {
        //cpu.trace(*mem);
        auto const result = cpu.exec(*mem);
        switch (result) {
        case CPU::Result::OK:
            break;
        case CPU::Result::BAD:
            printf("Bad instruction!\n");
            return 0;
        case CPU::Result::HALT:
            printf("Halt!\n");
            return 0;
        case CPU::Result::STOP:
            printf("Stop!\n");
            return 0;
        case CPU::Result::INTERRUPT_DISABLE:
            break;
        case CPU::Result::INTERRUPT_ENABLE:
            break;
        }
    }
    return 0;
}
