#include "instructions.hpp"
#include "emulator.hpp"

namespace instruction {
    void jr(emulator_t *emu, const instruction_t &ins) noexcept {
        if (ins.operands[0].type == operand_type::condition) {
            i8 offset = std::bit_cast<i8>(ins.operands[1].i_8);
            if (emu->is_cond(emu->u8_cond(ins.operands[0].condition))) {
                emu->m_cycle();
                emu->pc += offset;
            }
            // Taken: 3 cycles
            // Untaken: 2 cycles
        } else if (ins.operands[0].type == operand_type::i_8) {
            i8 offset = std::bit_cast<i8>(ins.operands[0].i_8);
            emu->m_cycle();
            emu->pc += offset;
            // 3 cycles
        }
    }
}
