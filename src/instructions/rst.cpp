#include "instructions.hpp"
#include "emulator.hpp"

namespace instruction {
    void rst(emulator_t *emu, const instruction_t &ins) noexcept {
        u8 rst_vec = ins.operands[0].i_8;
        u8 premul_addr = 0;
        switch (rst_vec) {
            default:
                break;
            case 0:
                premul_addr = 0x00;
                break;
            case 1:
                premul_addr = 0x08;
                break;
            case 2:
                premul_addr = 0x10;
                break;
            case 3:
                premul_addr = 0x18;
                break;
            case 4:
                premul_addr = 0x20;
                break;
            case 5:
                premul_addr = 0x28;
                break;
            case 6:
                premul_addr = 0x30;
                break;
            case 7:
                premul_addr = 0x38;
                break;
        }

        emu->call(premul_addr * 8);
    }
}

