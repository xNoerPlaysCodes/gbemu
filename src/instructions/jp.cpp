#include "instructions.hpp"
#include "emulator.hpp"

namespace instruction {
    void jp(emulator_t *emu, const instruction_t &ins) noexcept {
        if (ins.opcode == 0xE9) { // jp hl
            emu->pc = emu->hl;
            return;
        }

        if (ins.operands[0].type == operand_type::memory_address) {
            emu->pc = ins.operands[0].memory_address;
            emu->m_cycle(); // takes 4 cycles instead of what i 
                            // expected (3)
        } else if (ins.operands[0].type == operand_type::) {
            // TODO: Implement
        }
    }
}
