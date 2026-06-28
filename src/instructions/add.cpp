#include "instructions.hpp"
#include "emulator.hpp"
#include "bitutil.hpp"

namespace instruction {
    void add(emulator_t *emu, const instruction_t &ins) noexcept {
        if (ins.operands[0].type == operand_type::r_16) {
            u16 *dst = emu->r16_to_reg(ins.operands[0].r_16);
            if (ins.operands[1].type == operand_type::r_16) {
                u16 *src = emu->r16_to_reg(ins.operands[1].r_16);

                u32 sum = *dst + *src;
                emu->set_af_c((sum >> 16) & 1);
                emu->set_af_h(((*dst & 0x0FFF) + (*src & 0x0FFF)) >> 12);

                emu->m_cycle(); // takes 2 cycles :(

                *dst = sum;
            } else if (ins.operands[1].type == operand_type::i_8) {
                if (dst == &emu->sp) {
                    i8 signed_value = std::bit_cast<i8>(ins.operands[1].i_8);
                    *dst += signed_value;

                    emu->set_af_z(0);
                    emu->set_af_n(0);
                    u16 sum1 = ((emu->sp & 0x0F) + (signed_value & 0x0F));
                    u16 sum2 = ((emu->sp & 0xFF) + (signed_value & 0xFF));
                    emu->set_af_h(sum1 > 0x0F);
                    emu->set_af_c(sum2 > 0xFF);
                }
            }
        } else if (ins.operands[1].type == operand_type::r_8) {
            u8 *dst = emu->r8_to_reg(ins.operands[0].r_8);
            if (ins.operands[1].type == operand_type::r_8) {
                u8 *src = emu->r8_to_reg(ins.operands[1].r_8);
                u8 u8sum = *dst + *src;
                emu->set_af_z(u8sum == 0);
                emu->set_af_n(0);
                u16 sum1 = ((*dst & 0x7) + (*src & 0x7));
                u16 sum2 = ((*dst & 0x7F) + (*src & 0x7F));
                emu->set_af_h(sum1 > 0x7);
                emu->set_af_c(sum2 > 0x7F);
                *dst = u8sum;
            } else if (ins.operands[1].type == operand_type::memory_address_reg) {
                u8 src = emu->read8(*emu->r16mem_to_reg(ins.operands[1].memory_address_reg));
                u8 u8sum = *dst + src;
                emu->set_af_z(u8sum == 0);
                emu->set_af_n(0);
                u16 sum1 = ((*dst & 0x7) + (src & 0x7));
                u16 sum2 = ((*dst & 0x7F) + (src & 0x7F));
                emu->set_af_h(sum1 > 0x7);
                emu->set_af_c(sum2 > 0x7F);
                *dst = u8sum;
            } else if (ins.operands[1].type == operand_type::i_8) {
                u8 src = ins.operands[1].i_8;
                u8 u8sum = *dst + src;
                emu->set_af_z(u8sum == 0);
                emu->set_af_n(0);
                u16 sum1 = ((*dst & 0x7) + (src & 0x7));
                u16 sum2 = ((*dst & 0x7F) + (src & 0x7F));
                emu->set_af_h(sum1 > 0x7);
                emu->set_af_c(sum2 > 0x7F);
                *dst = u8sum;
            }
        }

        emu->set_af_n(0);
    }
}
