#include "instructions.hpp"
#include "emulator.hpp"
#include "bitutil.hpp"

namespace instruction {
    void adc(emulator_t *emu, const instruction_t &ins) noexcept {
        if (ins.operands[1].type == operand_type::r_8) {
            u8 carry = emu->af_c();
            u8 *a = reinterpret_cast<u8*>(emu->af) + 1;
            if (ins.operands[1].type == operand_type::r_8) {
                u8 *src = emu->r8_to_reg(ins.operands[1].r_8);
                u8 u8sum = carry + *src;
                emu->set_af_z(u8sum == 0);
                emu->set_af_n(0);
                u16 sum1 = ((carry & 0x7) + (*src & 0x7));
                u16 sum2 = ((carry & 0x7F) + (*src & 0x7F));
                emu->set_af_h(sum1 > 0x7);
                emu->set_af_c(sum2 > 0x7F);
                *a = u8sum;
            } else if (ins.operands[1].type == operand_type::memory_address_reg) {
                u8 src = emu->read8(*emu->r16mem_to_reg(ins.operands[1].memory_address_reg));
                u8 u8sum = carry + src;
                emu->set_af_z(u8sum == 0);
                emu->set_af_n(0);
                u16 sum1 = ((carry & 0x7) + (src & 0x7));
                u16 sum2 = ((carry & 0x7F) + (src & 0x7F));
                emu->set_af_h(sum1 > 0x7);
                emu->set_af_c(sum2 > 0x7F);
                *a = u8sum;
            } else if (ins.operands[1].type == operand_type::i_8) {
                u8 src = ins.operands[1].i_8;
                u8 u8sum = carry + src;
                emu->set_af_z(u8sum == 0);
                emu->set_af_n(0);
                u16 sum1 = ((carry & 0x7) + (src & 0x7));
                u16 sum2 = ((carry & 0x7F) + (src & 0x7F));
                emu->set_af_h(sum1 > 0x7);
                emu->set_af_c(sum2 > 0x7F);
                *a = u8sum;
            }
        }

        emu->set_af_n(0);
    }
}
