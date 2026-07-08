#include "instructions.hpp"
#include "emulator.hpp"

namespace instruction {
    void ei(emulator_t *emu, const instruction_t &) noexcept {
        emu->ime = 1;
    }
}
