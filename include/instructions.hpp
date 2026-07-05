#pragma once

#include "types.hpp"

struct emulator_t;

enum class operand_type : u8 {
    null = 0,
    memory_address,
    memory_address_reg,
    i_16,
    i_8,
    r_16,
    r_8,
    condition
};

struct instruction_t {
    struct operand_t {
        union {
            u16 memory_address;
            u16 memory_address_reg;
            u16 i_16;
            u8 i_8;
            u16 r_16;
            u8 r_8;
            u8 condition;
        };
        operand_type type = operand_type::null;
    };

    void (*handler)(emulator_t *, const instruction_t &) = nullptr;
    u16 cur_pc = 0;
    u8 opcode = 0;
    u8 bytes = 0;
    operand_t operands[2] = {};
};

namespace instruction {
    void nop(emulator_t *, const instruction_t &) noexcept;
    void ld(emulator_t *, const instruction_t &) noexcept;
    void inc(emulator_t *, const instruction_t &) noexcept;

    // grp_bits
    void rlca(emulator_t *, const instruction_t &) noexcept;
    void rrca(emulator_t *, const instruction_t &) noexcept;
    void rla(emulator_t *, const instruction_t &) noexcept;
    void rra(emulator_t *, const instruction_t &) noexcept;
    void daa(emulator_t *, const instruction_t &) noexcept;
    void cpl(emulator_t *, const instruction_t &) noexcept;
    void scf(emulator_t *, const instruction_t &) noexcept;
    void ccf(emulator_t *, const instruction_t &) noexcept;

    void add(emulator_t *, const instruction_t &) noexcept;
    void sub(emulator_t *, const instruction_t &) noexcept;
    void adc(emulator_t *, const instruction_t &) noexcept;
    void sbc(emulator_t *, const instruction_t &) noexcept;
    void or_(emulator_t *, const instruction_t &) noexcept;
    void and_(emulator_t *, const instruction_t &) noexcept;
    void xor_(emulator_t *, const instruction_t &) noexcept;
    void cp(emulator_t *, const instruction_t &) noexcept;

    void jp(emulator_t *, const instruction_t &) noexcept;
    void jr(emulator_t *, const instruction_t &) noexcept;
    void call(emulator_t *, const instruction_t &) noexcept;
    void ret(emulator_t *, const instruction_t &) noexcept;
    void reti(emulator_t *, const instruction_t &) noexcept;
    void rst(emulator_t *, const instruction_t &) noexcept;
}
