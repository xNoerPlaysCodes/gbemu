#include "instructions.hpp"
#include "emulator.hpp"

namespace instruction {
    /*
     * i_8 -> memory_address
i_8 -> memory_address_reg

r_8 -> memory_address
r_8 -> memory_address_reg

memory_address -> r_8
memory_address_reg -> r_8

r_8 -> r_8
r_16 -> r_16
*/
    void ld(emulator_t *emu, const instruction_t &ins) noexcept {
        if (ins.operands[0].type == operand_type::r_8) {
            u8 *dst = emu->r8_to_reg(ins.operands[0].r_8);
            if (ins.operands[1].type == operand_type::r_8) {
                u8 *src = emu->r8_to_reg(ins.operands[1].r_8);
                *dst = *src;
            } else if (ins.operands[1].type == operand_type::i_8) {
                *dst = ins.operands[1].i_8;
            } else if (ins.operands[1].type == operand_type::memory_address) {
                u8 src = emu->read8(ins.operands[1].memory_address);
                *dst = src;
            } else if (ins.operands[1].type == operand_type::memory_address_reg) {
                u8 data = emu->read8(*emu->r16mem_to_reg(ins.operands[1].r_16));
                *dst = data;
            } 

            return;
        } else if (ins.operands[0].type == operand_type::r_16) {
            u16 *dst = emu->r16_to_reg(ins.operands[0].r_16);
            if (ins.operands[1].type == operand_type::i_16) {
                *dst = ins.operands[1].i_16;
            } else if (ins.operands[1].type == operand_type::memory_address) {
                u16 src = emu->read8(ins.operands[1].memory_address);
                *dst = src;
            } else if (ins.operands[1].type == operand_type::memory_address_reg) {
                u8 src = emu->read8(*emu->r16mem_to_reg(ins.operands[1].memory_address_reg));
                *dst = src;
            } else if (ins.operands[1].type == operand_type::r_8) {
                *dst = *emu->r8_to_reg(ins.operands[1].r_8);
            }
            return;
        } else if (ins.operands[0].type == operand_type::memory_address_reg) {
            u16 *dst = emu->r16mem_to_reg(ins.operands[0].memory_address_reg);
            if (ins.operands[1].type == operand_type::r_8) {
                u8 *src = emu->r8_to_reg(ins.operands[1].memory_address);
                emu->write8(*dst, *src);
            } else if (ins.operands[1].type == operand_type::r_16) {
                u16 *src = emu->r16_to_reg(ins.operands[1].r_16);
                emu->write8(*dst, *src);
            } else if (ins.operands[1].type == operand_type::i_16) {
                emu->write8(*dst, ins.operands[1].i_16);
            } else if (ins.operands[1].type == operand_type::i_8) {
                emu->write8(*dst, ins.operands[1].i_8);
            }
        } else if (ins.operands[0].type == operand_type::memory_address) {
            u16 dst = ins.operands[1].memory_address;
            if (ins.operands[1].type == operand_type::r_8) {
                emu->write8(dst, *emu->r8_to_reg(ins.operands[1].r_8));
            } else if (ins.operands[1].type == operand_type::r_16) {
                emu->write8(dst, *emu->r16_to_reg(ins.operands[1].r_16));
            } else if (ins.operands[1].type == operand_type::i_16) {
                emu->write8(dst, ins.operands[1].i_16);
            }
        }
    }
}
