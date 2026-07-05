#include "instructions.hpp"
#include "emulator.hpp"

namespace instruction {
    void dec(emulator_t *emu, const instruction_t &ins) noexcept {
        if (ins.operands[0].type == operand_type::r_16) {
            u16 *reg = emu->r16_to_reg(ins.operands[0].r_16);
            (*reg)--;

            emu->set_af_z(*reg == 0);
        } else if (ins.operands[0].type == operand_type::r_8) {
            u8 *reg = emu->r8_to_reg(ins.operands[0].r_8);
            emu->set_af_h(((*reg & 0x07) < 1));
            (*reg)--;

            emu->set_af_z(*reg == 0);
        }

        emu->set_af_n(0);
    }
}

