#include "instructions.hpp"
#include "emulator.hpp"

namespace instruction {
    void actual_call(emulator_t *emu, u16 addr) {
        emu->push16(emu->pc);
        emu->m_cycle(2);
        instruction::jp(emu, {
            .handler = jp,
            .cur_pc = 0x0,
            .opcode = 0x0,
            .bytes = 0x0,
            .operands = {
                {
                    .i_16 = addr,
                    .type = operand_type::i_16
                }
            },
        });
    }

    void call(emulator_t *emu, const instruction_t &ins) noexcept {
        if (ins.operands[0].type == operand_type::i_16) {
            actual_call(emu, ins.operands[0].i_16);
        } else if (ins.operands[0].type == operand_type::condition) {
            if (emu->is_cond(emu->u8_cond(ins.operands[0].condition))) {
                actual_call(emu, ins.operands[1].i_16);
            }
        }
    }
}

