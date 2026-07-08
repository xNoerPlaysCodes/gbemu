#include "instructions.hpp"
#include "emulator.hpp"

namespace instruction {
    void reti(emulator_t *emu, const instruction_t &ins) noexcept {
        emu->m_cycle();
        emu->ime = 1;
        emu->ret();
    }
}