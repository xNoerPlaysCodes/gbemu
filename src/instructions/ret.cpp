#include "instructions.hpp"
#include "emulator.hpp"

namespace instruction {
    void ret(emulator_t *emu, const instruction_t &ins) noexcept {
        if (ins.operands[0].type == operand_type::condition) {
            emu->m_cycle();
            if (emu->is_cond(emu->u8_cond(ins.operands[0].condition))) {
                emu->m_cycle();
                emu->ret();
            }
        }

        if (ins.opcode == 0xC9) { // ret
            emu->m_cycle();
            emu->ret();
        } else if (ins.opcode == 0xD9) { // reti
            emu->m_cycle();
            emu->ime = 1;
            emu->ret();
        } 
    }
}

