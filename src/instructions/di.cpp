#include "instructions.hpp"
#include "emulator.hpp"

namespace instruction {
    void di(emulator_t *emu, const instruction_t &) noexcept {
        emu->ime = 0;
    }
}
