#pragma once

#include <raylib.h>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

#include "instructions.hpp"
#include "types.hpp"
#include "globals.hpp"

#include <cassert>

extern u8 free_reign[4096];
extern u16 scratchpad[16];
extern u8 scratchpad_ptr;

template<typename T>
struct cycler;

namespace toggle::cycle {
    constexpr bool cycle = true;
    constexpr bool no_cycle = false;
}

struct emulator_t {
public:
    static u8 boot_room[256];
    static constexpr u8 int_vblank  = 0x40;
    static constexpr u8 int_stat     = 0x48;
    static constexpr u8 int_timer   = 0x50;
    static constexpr u8 int_serial  = 0x58;
    static constexpr u8 int_joypad  = 0x60;

    // 23.04 KB
    u8 fb[fb_x * fb_y] = {};

    std::vector<u8> cartridge;
    u8 wram[8192] = {};
    u8 vram[8192] = {};
    u8 hram[128] = {};

    u16 int_handlers[5] = {};

    u64 clock = 0;
    u8 ime = 0;
    u8 div = 0xAB; // 16384 Hz
    u8 tima = 0; // specified by tma
    u8 tac = 0xF8;
    u8 tma = 0;

    enum class tima_state_e {
        normal,
        pending
    } tima_state = tima_state_e::normal;

    u8 lcd_control = 0x91;

    u8 ly = 0;
    u8 lyc = 0;
    u8 stat = 0x85;

    u8 bank = 0x01;

    u8 ie = 0x00;
    u8 _if = 0xE1;

    bool stop_mode = false;

    u16 get_interrupt_handler(u8 int_) noexcept;

    void t_cycle(i32 n = 1) noexcept;

    void m_cycle(i32 n = 1) noexcept;

    void handle_interrupts() noexcept;

    void request_interrupt(u8 int_) noexcept;

    void interrupts() noexcept;

    void incr_tima() noexcept;

    u16 af = 0x01B0;
    u16 bc = 0x0013;
    u16 de = 0x00D8;
    u16 hl = 0x0148;
    u16 sp = 0xFFFE;
    u16 pc = 0x0100;

    void push(u8 value) noexcept;
    void push16(u16 value) noexcept;

    u8 pop() noexcept;
    u16 pop16() noexcept;

    void call(u16 addr) noexcept;

    void ret() noexcept;

    inline constexpr u8 b() const noexcept {
        return bc >> 8; // IS THIS HIGH BYTE OR WHAT
    }

    inline constexpr u8 c() const noexcept {
        return bc & 0xFF;
    }

    inline constexpr u8 d() const noexcept {
        return de >> 8;
    }

    inline constexpr u8 e() const noexcept {
        return de & 0xFF;
    }

    inline constexpr u8 h() const noexcept {
        return hl >> 8;
    }

    inline constexpr u8 l() const noexcept {
        return hl & 0xFF;
    }

    inline constexpr u8 af_high() const noexcept {
        return af >> 8;
    }

    inline constexpr u8 af_low() const noexcept {
        return af & 0xFF;
    }

    inline constexpr u8 af_z() const noexcept {
        return (af_low() >> 7) & 1;
    }

    inline constexpr u8 af_n() const noexcept {
        return (af_low() >> 6) & 1;
    }

    inline constexpr u8 af_h() const noexcept {
        return (af_low() >> 5) & 1;
    }

    inline constexpr u8 af_c() const noexcept {
        return (af_low() >> 4) & 1;
    }

    inline constexpr void set_af_c(u8 bit) noexcept {
        if (bit) {
            af |= 1 << 12;
        } else {
            af &= ~(1 << 12);
        }
    }

    inline constexpr void set_af_h(u8 bit) noexcept {
        if (bit) {
            af |= 1 << 13;
        } else {
            af &= ~(1 << 13);
        }
    }

    inline constexpr void set_af_n(u8 bit) noexcept {
        if (bit) {
            af |= 1 << 14;
        } else {
            af &= ~(1 << 14);
        }
    }

    inline constexpr void set_af_z(u8 bit) noexcept {
        if (bit) {
            af |= 1 << 15;
        } else {
            af &= ~(1 << 15);
        }
    }
public:
    u8* memory(u16 addr) noexcept;

    u8 read8(u16 addr, bool cycle = true) noexcept;

    void write8(u16 addr, u8 value, bool cycle = true) noexcept;
public:
    // Start: 4 so 4, 5
    inline constexpr u8 get_r16(u8 byte, u8 start) {
        return (byte >> start) & 0b11;
    }

    inline constexpr u8 get_r8(u8 byte, u8 start) {
        return (byte >> start) & 0b111;
    }

    inline constexpr u8 get_cond(u8 byte, u8 start) {
        return (byte >> start) & 0b11;
    }

    inline constexpr u16* r16_to_reg(u8 r16) {
        switch (r16) {
            case 0:
                return &bc;
            case 1:
                return &de;
            case 2:
                return &hl;
            case 3:
                return &sp;
            default: return nullptr;
        }
    }

    inline constexpr u16* r16stk_to_reg(u8 r16stk) {
        switch (r16stk) {
            case 0:
                return &bc;
            case 1:
                return &de;
            case 2:
                return &hl;
            case 3:
                return &af;
            default: return nullptr;
        }
    }

    inline constexpr u16* r16mem_to_reg(u8 r16mem) {
        switch (r16mem) {
            case 0:
                return &bc;
            case 1:
                return &de;
            case 2:
                scratchpad[scratchpad_ptr % 16] = hl++;
                return &scratchpad[scratchpad_ptr++];
            case 3:
                scratchpad[scratchpad_ptr % 16] = hl--;
                return &scratchpad[scratchpad_ptr++];
            default: return nullptr;
        }
    }

    inline constexpr u8* r8_to_reg(u8 r8) {
        switch (r8) {
            case 0:
                return reinterpret_cast<u8*>(&bc) + 1;
            case 1:
                return reinterpret_cast<u8*>(&bc);
            case 2:
                return reinterpret_cast<u8*>(&de) + 1;
            case 3:
                return reinterpret_cast<u8*>(&de);
            case 4:
                return reinterpret_cast<u8*>(&hl) + 1;
            case 5:
                return reinterpret_cast<u8*>(&hl);
            case 6:
                return memory(hl);
            case 7:
                return reinterpret_cast<u8*>(&af) + 1;
            default: return nullptr;
        }
    }

    enum class cond {
        null = 0,
        notzero,
        zero,
        notcarry,
        carry
    };

    inline constexpr cond u8_cond(u8 u8) const noexcept {
        switch (u8) {
            case 0:
                return cond::notzero;
            case 1:
                return cond::zero;
            case 2:
                return cond::notcarry;
            case 3:
                return cond::carry;
            default:
                return cond::null;
        }
    }

    bool is_cond(cond c) const noexcept;

    inline constexpr u16 le_combine(u8 low, u8 high) {
        return (high << 8) | low;
    }

    instruction_t decode(u8 opcode, cycler<u8> n_plus_1, cycler<u8> n_plus_2) noexcept;
public:
    void logic() noexcept;
private:
    struct ppu {
        void tick() noexcept;
    } ppu;
public:
    /// @brief Initialize with cartridge
    void init(std::vector<u8> &&bytes) noexcept;

    void step(u32 n = 1) noexcept;
};

template<typename T>
struct cycler {
private:
    T value;
public:
    emulator_t *emu = nullptr;
public:
    T view() noexcept {
        emu->m_cycle();
        return value;
    }

    operator T() noexcept {
        return view();
    }

    cycler& operator=(T value) noexcept = delete;

    cycler() = default;
    cycler(T v) : value(v) {}
    cycler(emulator_t *emu, T v) : value(v), emu(emu) {}
};
