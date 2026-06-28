#include "bitutil.hpp"
#include "instructions.hpp"
#include "emulator.hpp"

namespace instruction {
    void rlca(emulator_t *emu, const instruction_t &ins) noexcept {
        u8 *byte = emu->r8_to_reg(ins.operands[0].r_8);
        u8 old = *byte;
        u8 bit7 = (old >> 7) & 1;
        *byte = ((old << 1) | bit7) & 0xFF;
        emu->set_af_c(bit7);
        
        emu->set_af_z(0);
        emu->set_af_n(0);
        emu->set_af_h(0);
    }

    void rrca(emulator_t *emu, const instruction_t &ins) noexcept {
        u8 *byte = emu->r8_to_reg(ins.operands[0].r_8);
        u8 old = *byte;
        u8 bit7 = *byte & 1;
        *byte = (old >> 1) | (bit7 << 7);
        emu->set_af_c(bit7);

        emu->set_af_z(0);
        emu->set_af_n(0);
        emu->set_af_h(0);
    }

    void rla(emulator_t *emu, const instruction_t &ins) noexcept {
        u8 *byte = emu->r8_to_reg(ins.operands[0].r_8);
        u8 old_c = emu->af_c();
        u8 new_c = (*byte >> 7) & 1;

        *byte <<= 1;
        if (old_c) {
            *byte |= 1 << 0;
        } else {
            *byte &= ~(1 << 0);
        }
        
        emu->set_af_c(new_c);
        *byte &= 0xFF;

        emu->set_af_z(0);
        emu->set_af_n(0);
        emu->set_af_h(0);
    }

    void rra(emulator_t *emu, const instruction_t &ins) noexcept {
        u8 *byte = emu->r8_to_reg(ins.operands[0].r_8);
        u8 old_c = emu->af_c();
        u8 new_c = *byte & 1;

        *byte >>= 1;
        *byte = bitutil::change(*byte, 7, old_c);

        emu->set_af_c(new_c);
        *byte &= 0xFF;

        emu->set_af_z(0);
        emu->set_af_n(0);
        emu->set_af_h(0);
    }

    void daa(emulator_t *emu, const instruction_t &ins) noexcept {
        spdlog::critical("I AM NOT IMPLEMENTING THIS SHIT (instr:daa)");
    }

    void cpl(emulator_t *emu, const instruction_t &ins) noexcept {
        u8 *byte = emu->r8_to_reg(ins.operands[0].r_8);
        *byte = ~(*byte);

        emu->set_af_n(1);
        emu->set_af_h(1);
    }

    void scf(emulator_t *emu, const instruction_t &ins) noexcept {
        emu->set_af_c(1);
        emu->set_af_n(0);
        emu->set_af_h(0);
    }

    void ccf(emulator_t *emu, const instruction_t &ins) noexcept {
        emu->set_af_c(!emu->af_c());
        emu->set_af_n(0);
        emu->set_af_h(0);
    }
}
