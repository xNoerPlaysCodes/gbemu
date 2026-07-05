#include "instructions.hpp"
#include "emulator.hpp"

namespace instruction {
    void push(emulator_t *emu, const instruction_t &ins) noexcept {
        emu->push16(*(emu->r16stk_to_reg(ins.operands[0].r_8)));
    }
}

