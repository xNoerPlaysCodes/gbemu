#include "instructions.hpp"
#include "emulator.hpp"

namespace instruction {
    void jp(emulator_t *emu, const instruction_t &ins) noexcept {
        if (ins.opcode == 0xE9) { // jp hl
            emu->pc = emu->hl;
            return;
        }

        if (ins.operands[0].type == operand_type::condition) {
            if (emu->is_cond(emu->u8_cond(ins.operands[0].condition))) {
                emu->m_cycle();
                emu->pc = ins.operands[1].i_16;
            }
        } else if (ins.operands[0].type == operand_type::i_16) {
            emu->m_cycle();
            emu->pc = ins.operands[0].i_16;
        }
    }
}

