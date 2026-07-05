#include "instructions.hpp"
#include "emulator.hpp"

namespace instruction {
    void pop(emulator_t *emu, const instruction_t &ins) noexcept {
        *(emu->r16stk_to_reg(ins.operands[0].r_8)) = emu->pop16();
    }
}

