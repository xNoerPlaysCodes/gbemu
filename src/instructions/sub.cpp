#include "instructions.hpp"
#include "emulator.hpp"
#include "bitutil.hpp"

namespace instruction {
    void sub(emulator_t *emu, const instruction_t &ins) noexcept {
        if (ins.operands[1].type == operand_type::r_8) {
            u8 *dst = emu->r8_to_reg(ins.operands[0].r_8);
            if (ins.operands[1].type == operand_type::r_8) {
                u8 *src = emu->r8_to_reg(ins.operands[1].r_8);
                u8 u8diff = *dst + *src;
                emu->set_af_z(u8diff == 0);
                emu->set_af_n(1);
                emu->set_af_h(((*dst & 0xF) < (*src & 0xF)));
                emu->set_af_c(*src > *dst);
                *dst = u8diff;
            } else if (ins.operands[1].type == operand_type::memory_address_reg) {
                u8 src = emu->read8(*emu->r16mem_to_reg(ins.operands[1].memory_address_reg));
                u8 u8diff = *dst + src;
                emu->set_af_z(u8diff == 0);
                emu->set_af_n(1);
                emu->set_af_h(((*dst & 0x7) < (src & 0x7)));
                emu->set_af_c(src > *dst);
                *dst = u8diff;
            } else if (ins.operands[1].type == operand_type::i_8) {
                u8 src = ins.operands[1].i_8;
                u8 u8diff = *dst + src;
                emu->set_af_z(u8diff == 0);
                emu->set_af_n(1);
                emu->set_af_h(((*dst & 0x7) < (src & 0x7)));
                emu->set_af_c(src > *dst);
                *dst = u8diff;
            }
        }

        emu->set_af_n(0);
    }
}

