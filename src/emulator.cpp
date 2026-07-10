    #include "bitutil.hpp"
    #include "instructions.hpp"
    #include "types.hpp"
    #include "globals.hpp"
    #include <string_view>
    #include "emulator.hpp"

    u8 free_reign[4096] = {};
    u16 scratchpad[16] = {};
    u8 scratchpad_ptr = 0;

    u8 emulator_t::boot_room[256] = {
        0x31, 0xFE, 0xFF, 0xAF, 0x21, 0xFF, 0x9F, 0x32, 0xCB, 0x7C, 0x20, 0xFB, 0x21, 0x26, 0xFF, 0x0E, 0x11, 0x3E, 0x80, 0x32, 0xE2, 0x0C, 0x3E, 0xF3, 0xE2, 0x32, 0x3E, 0x77, 0x77, 0x3E, 0xFC, 0xE0
    , 0x47, 0x11, 0x04, 0x01, 0x21, 0x10, 0x80, 0x1A, 0xCD, 0x95, 0x00, 0xCD, 0x96, 0x00, 0x13, 0x7B, 0xFE, 0x34, 0x20, 0xF3, 0x11, 0xD8, 0x00, 0x06, 0x08, 0x1A, 0x13, 0x22, 0x23, 0x05, 0x20, 0xF9, 0x3E, 0x19, 0xEA, 0x10, 0x99, 0x21, 0x2F, 0x99, 0x0E, 0x0C, 0x3D, 0x28, 0x08, 0x32, 0x0D, 0x20, 0xF9, 0x2E, 0x0F, 0x18, 0xF3, 0x67, 0x3E, 0x64, 0x57, 0xE0, 0x42, 0x3E, 0x91, 0xE0, 0x40,
    0x04, 0x1E, 0x02, 0x0E, 0x0C, 0xF0, 0x44, 0xFE, 0x90, 0x20, 0xFA, 0x0D, 0x20, 0xF7, 0x1D, 0x20, 0xF2, 0x0E, 0x13, 0x24, 0x7C, 0x1E, 0x83, 0xFE, 0x62, 0x28, 0x06, 0x1E, 0xC1, 0xFE, 0x64, 0x20
    , 0x06, 0x7B, 0xE2, 0x0C, 0x3E, 0x87, 0xE2, 0xF0, 0x42, 0x90, 0xE0, 0x42, 0x15, 0x20, 0xD2, 0x05, 0x20, 0x4F, 0x16, 0x20, 0x18, 0xCB, 0x4F, 0x06, 0x04, 0xC5, 0xCB, 0x11, 0x17, 0xC1, 0xCB, 0x11, 0x17, 0x05, 0x20, 0xF5, 0x22, 0x23, 0x22, 0x23, 0xC9, 0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D, 0x00, 0x08, 0x11, 0x1F, 0x88, 0x89,
    0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99, 0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E, 0x3C, 0x42, 0xB9, 0xA5, 0xB9, 0xA5
    , 0x42, 0x3C, 0x21, 0x04, 0x01, 0x11, 0xA8, 0x00, 0x1A, 0x13, 0xBE, 0x20, 0xFE, 0x23, 0x7D, 0xFE, 0x34, 0x20, 0xF5, 0x06, 0x19, 0x78, 0x86, 0x23, 0x05, 0x20, 0xFB, 0x86, 0x20, 0xFE, 0x3E, 0x01, 0xE0, 0x50
    };

    u16 emulator_t::get_interrupt_handler(u8 int_) noexcept {
        switch (int_) {
            case int_vblank:
                return int_handlers[0];
            case int_stat:
                return int_handlers[1];
            case int_timer:
                return int_handlers[2];
            case int_serial:
                return int_handlers[3];
            case int_joypad:
                return int_handlers[4];
            default: return 0;
        }
    }

    void emulator_t::t_cycle(i32 n) noexcept {
        for (i32 i = 0; i < n; ++i) {
            ++clock;
            ppu.tick();
            [[likely]];
            [[unlikely]];
        }
    }

    void emulator_t::m_cycle(i32 n) noexcept {
        t_cycle(n * 4);
    }

    void emulator_t::handle_interrupts() noexcept {
        if (ime == 0) return;

        ime = 0;

        auto call_handler = [&](u16 int_handler) -> void {
            m_cycle(2);
            call(int_handler);
        };

        if (_if & (1 << 0) && ie & (1 << 0)) {
            _if &= ~(1 << 0);
            call_handler(get_interrupt_handler(int_vblank));
            // vblank
        } else if (_if & (1 << 1) && ie & (1 << 1)) {
            _if &= ~(1 << 1);
            call_handler(get_interrupt_handler(int_stat));
            // lcd
        } else if (_if & (1 << 2) && ie & (1 << 2)) {
            _if &= ~(1 << 2);
            call_handler(get_interrupt_handler(int_timer));
            // timer
        } else if (_if & (1 << 3) && ie & (1 << 3)) {
            _if &= ~(1 << 3);
            call_handler(get_interrupt_handler(int_serial));
            // serial
        } else if (_if & (1 << 4) && ie & (1 << 4)) {
            _if &= ~(1 << 4);
            call_handler(get_interrupt_handler(int_joypad));
            // joypad
        }

        ime = 1;

    }

    void emulator_t::request_interrupt(u8 int_) noexcept {
        if (ime == 0) return;

        if (int_ == int_vblank /* vblank */) {
            // The VBlank interrupt occurs ca. 59.7 times a second
            // on a handheld Game Boy
            _if |= 1 << 0;
        } else if (int_ == int_stat /* STAT / LCD */) {
            _if |= 1 << 1;
        } else if (int_ == int_timer /* Timer */) {
            _if |= 1 << 2;
        } else if (int_ == int_serial /* Serial */) {
            _if |= 1 << 3;
        } else if (int_ == int_joypad /* Joypad */) {
            _if |= 1 << 4;
        }
    }

    void emulator_t::interrupts() noexcept {
        poll_joypad_state(&g_joypad_state);

        if (g_joypad_state & (1 << 4) || g_joypad_state & (1 << 5)) {
            request_interrupt(int_joypad);
        }
    }

    void emulator_t::incr_tima() noexcept {
        u8 lower_two_bits = tac & 0x03;
        auto incr_tima_logic = [&](u64 update_clock) -> void {
            if (tima_state == tima_state_e::pending) {
                tima = tma;
            }
            if (tima == 0xFF && tima_state == tima_state_e::normal) {
                tima_state = tima_state_e::pending;
            }
            if (tima_state != tima_state_e::pending && clock % update_clock == 0) {
                ++tima;
            }

            if (tac & (1 << 2))
                request_interrupt(0x50);

            // TODO: Emulate this behaviour:
            // If a TMA write is executed on the same M-cycle as the content of
            // TMA is transferred to TIMA due to a timer overflow, the old value
            // is transferred to TIMA.
            //    ~ gbdev.io/pandocs
        };
        if (lower_two_bits == 0) {
            incr_tima_logic(256);
        } else if (lower_two_bits == 1) {
            incr_tima_logic(4);
        } else if (lower_two_bits == 2) {
            incr_tima_logic(16);
        } else if (lower_two_bits == 3) {
            incr_tima_logic(64);
        }
    }

    void emulator_t::push(u8 value) noexcept {
        // assert(sp != 0);
        m_cycle();
        write8(--sp, value);
    }

    void emulator_t::push16(u16 value) noexcept {
        // assert(sp != 0);
        m_cycle();
        write8(--sp, value >> 8);
        write8(--sp, value & 0xFF); // 3
    }

    u8 emulator_t::pop() noexcept {
        assert(sp != UINT16_MAX);
        return read8(sp++);
    }

    u16 emulator_t::pop16() noexcept {
        assert(sp != UINT16_MAX);
        u8 low = read8(sp++);
        u8 high = read8(sp++);
        return (high >> 8) | low;
    }

    void emulator_t::call(u16 addr) noexcept {
        u16 next_pc = pc;
        push16(next_pc); // 3

        pc = addr;
    }

    void emulator_t::ret() noexcept {
        pc = pop16();
    }

    u8* emulator_t::memory(u16 addr) noexcept {
        static int _ = [&]() -> int {
            std::memset(free_reign, 0xDD, sizeof(free_reign)); // 0xDD is invalid instruction;
            return 0;
        } ();
    #define MMIO_BP(start_address, end_address) u16 start = start_address; addr >= start && addr <= end_address
        if (MMIO_BP(0x0, 0xFF)) {
            if (!bank)
                return &boot_room[addr - start];
            else
                return &cartridge.at(addr - start);
        } else if (MMIO_BP(0x100, 0x3FFF)) {
            return &cartridge[addr - start];
        } else if (MMIO_BP(0x4000, 0x7FFF)) {
            spdlog::critical("ROM BANK");
            // TODO: Handle ROM BANK
        } else if (MMIO_BP(0x8000, 0x9FFF)) {
            return &vram[addr - start];
        } else if (MMIO_BP(0xA000, 0xBFFF)) {
            spdlog::critical("ROM Switchable EXternal RAM"); // TODO
        } else if (MMIO_BP(0xC000, 0xDFFF)) {
            return &wram[addr - start];
        } else if (MMIO_BP(0xE000, 0xFDFF)) {
            return &wram[addr - start]; // Mirror of region [0xC000-0xDDFF]
        } else if (MMIO_BP(0xFE00, 0xFE9F)) {
            spdlog::critical("Object Attribute Memory"); // TODO
        } else if (MMIO_BP(0xFEA0, 0xFEFF)) {
            // Unusable memory
        } else if (MMIO_BP(0xFF00, 0xFF7F)) {
            // TODO
            spdlog::critical("IO Registers");
            if (addr == 0xFF00) {
                poll_joypad_state(&g_joypad_state);
                return &g_joypad_state;
            } else if (addr >= 0xFF01 && addr <= 0xFF02) {
                spdlog::critical("Serial Transfer"); // TODO
            } else if (MMIO_BP(0xFF04, 0xFF07)) {
                if (addr == 0xFF04) {
                    return &div;
                } else if (addr == 0xFF05) {
                    return &tima;
                } else if (addr == 0xFF06) {
                    return &tma;
                } else if (addr == 0xFF07) {
                    return &tac;
                }
            } else if (addr == 0xFF0F) {
                return &_if;
            } else if (MMIO_BP(0xFF10, 0xFF26)) {
                spdlog::critical("Audio registers"); // TODO
            } else if (MMIO_BP(0xFF30, 0xFF3F)) {
                spdlog::critical("Wave RAM"); // TODO
            } else if (MMIO_BP(0xFF40, 0xFF4B)) {
                if (addr == 0xFF44) {
                    return &ly;
                } else if (addr == 0xFF45) {
                    return &lyc;
                } else if (addr == 0xFF41) {
                    return &stat;
                }
            } else if (addr == 0xFF46) {
                spdlog::critical("OAM DMA Transfer"); // TODO
            } else if (addr == 0xFF50) {
                return &bank;
            }
        } else if (MMIO_BP(0xFF80, 0xFFFE)) {
            return &hram[start];
        } else if (MMIO_BP(0xFFFF, 0xFFFF)) {
            return &ie;
        }

        return free_reign;
    }

    u8 emulator_t::read8(u16 addr, bool cycle) noexcept {
        u8 *mem = memory(addr);
        assert(mem != nullptr);
        if (mem == free_reign) {
            spdlog::critical("Memory is free reign");
        }
        if (cycle)
            m_cycle();
        return *mem;
    }

    void emulator_t::write8(u16 addr, u8 value, bool cycle) noexcept {
        u8 *mem = memory(addr);
        assert(mem != nullptr);
        if (mem == free_reign) {
            spdlog::critical("Memory is free reign");
        }

        *mem = value;
        spdlog::trace("Memory write 0x{:X} to 0x{:X} (Real Address = 0x{:X})", value, addr, (uintptr_t) (mem) + addr);
        if (cycle)
            m_cycle();

        if (mem == &div) {
            *mem = 0;
        }
    }

    static instruction_t decode_cb(u8) noexcept {
        return {
            .handler = [](emulator_t*, const instruction_t&) -> void {
                // TODO: Implement decode_cb
                spdlog::critical("TODO: Implement decode_cb");
                std::terminate();
            }
        };
    }

    static bool fits_pattern(std::string_view pattern, u8 value) {
        assert(pattern.size() == 8);
        for (size_t i = 0; i < pattern.size(); ++i) {
            char c = pattern[i];
            bool any = (c == 'x');
            bool _0 = (c == '0');
            bool _1 = (c == '1');

            if (any)
                continue;

            if (value & (1 << (7 - i))) {
                if (_0) {
                    return false;
                }
            } else {
                if (_1) {
                    return false;
                }
            }
        }

        return true;
    }

    instruction_t emulator_t::decode(u8 opcode, cycler<u8> n_plus_1, cycler<u8> n_plus_2) noexcept {
        namespace i = instruction;

        if (opcode == 0xCB)
            return decode_cb(n_plus_1);

        if (opcode == 0x0) {
            return {
                .handler = i::nop,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 1
            };
        } else if (opcode >= 0x40 && opcode <= 0x7F && opcode != 0x76) {
            // ld r8, r8
            return {
                .handler = i::ld,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 1,
                .operands = {
                    {
                        .r_8 = get_r8(opcode, 3),
                        .type = operand_type::r_8
                    },
                    {
                        .r_8 = get_r8(opcode, 0),
                        .type = operand_type::r_8
                    }
                },
            };
        } else if ( // ld r8, imm8
            opcode == 0x06 || opcode == 0x0E || opcode == 0x16 ||
            opcode == 0x1E || opcode == 0x26 || opcode == 0x2E ||
            opcode == 0x3E
        ) {
            return {
                .handler = i::ld,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 2,
                .operands = {
                    {
                        .r_8 = get_r8(opcode, 3),
                        .type = operand_type::r_8
                    },
                    {
                        .i_8 = n_plus_1,
                        .type = operand_type::i_8
                    }
                }
            };
        } else if ( // ld r16, imm16
            opcode == 0x01 || opcode == 0x11 || opcode == 0x21 ||
            opcode == 0x31
        ) {
            return {
                .handler = i::ld,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 3,
                .operands = {
                    {
                        .r_16 = get_r16(opcode, 4),
                        .type = operand_type::r_16
                    },
                    {
                        .i_16 = le_combine(n_plus_1, n_plus_2),
                        .type = operand_type::i_16
                    }
                }
            };
        } else if ( // ld a, [r16mem]
            opcode == 0x0A || opcode == 0x1A || opcode == 0x2A ||
            opcode == 0x3A
        ) {
            return {
                .handler = i::ld,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 1,
                .operands = {
                    {
                        .r_8 = 7, // reg a
                        .type = operand_type::r_8
                    },
                    {
                        .memory_address = get_r16(opcode, 4),
                        .type = operand_type::memory_address_reg
                    }
                }
            };
        } else if (
            opcode == 0x02 || opcode == 0x12 || opcode == 0x22 ||
            opcode == 0x32
        ) {
            return {
                .handler = i::ld,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 1,
                .operands = {
                    {
                        .memory_address = get_r16(opcode, 4),
                        .type = operand_type::memory_address_reg
                    },
                    {
                        .r_8 = 7, // reg a
                        .type = operand_type::r_8
                    },
                }
            };
        } else if (opcode == 0xFA) { // ld a, [a16]
            return {
                .handler = i::ld,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 3,
                .operands = {
                    {
                        .r_8 = 7, // reg a
                        .type = operand_type::r_8
                    },
                    {
                        .memory_address = le_combine(n_plus_1, n_plus_2),
                        .type = operand_type::memory_address
                    }
                }
            };
        } else if (opcode == 0xEA) { // ld [a16], a
            return {
                .handler = i::ld,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 3,
                .operands = {
                    {
                        .memory_address = le_combine(n_plus_1, n_plus_2),
                        .type = operand_type::memory_address
                    },
                    {
                        .r_8 = 7, // reg a
                        .type = operand_type::r_8
                    }
                }
            };
        } else if (opcode == 0xF0) { // ldh a, [n8]
            return {
                .handler = i::ld,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 2,
                .operands = {
                    {
                        .r_8 = 7, // reg a
                        .type = operand_type::r_8
                    },
                    {
                        .memory_address = u16(0xFF00 + n_plus_1),
                        .type = operand_type::memory_address
                    }
                }
            };
        } else if (opcode == 0xE0) { // ldh [n8], a
            return {
                .handler = i::ld,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 2,
                .operands = {
                    {
                        .memory_address = u16(0xFF00 + n_plus_1),
                        .type = operand_type::memory_address
                    },
                    {
                        .r_8 = 7, // reg a
                        .type = operand_type::r_8
                    },
                }
            };
        } else if (opcode == 0xF2) { // ldh a, [c]
            return {
                .handler = i::ld,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 1,
                .operands = {
                    {
                        .r_8 = 7, // reg a
                        .type = operand_type::r_8
                    },
                    {
                        .memory_address = u16(0xFF00 + c()),
                        .type = operand_type::memory_address
                    }
                }
            };
        } else if (opcode == 0xE2) { // ldh [c], a
            return {
                .handler = i::ld,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 1,
                .operands = {
                    {
                        .memory_address = u16(0xFF00 + c()),
                        .type = operand_type::memory_address
                    },
                    {
                        .r_8 = 7, // reg a
                        .type = operand_type::r_8
                    },
                }
            };
        } else if (opcode == 0xF8) { // ld hl, sp + r8
            return {
                // HACK: this is the only ld variant which affects flags
                //       and since ld just recieves the abstracted thing
                //       we need to handle setting flags ourselves
                .handler = [](emulator_t *emu, const instruction_t &ins) -> void {
                    i::ld(emu, ins);
                    emu->set_af_z(0);
                    emu->set_af_n(0);
                    u16 sum1 = ((emu->sp & 0x0F) + (bitutil::safe_sub(ins.operands[1].i_16, emu->sp) & 0x0F));
                    u16 sum2 = ((emu->sp & 0xFF) + (bitutil::safe_sub(ins.operands[1].i_16, emu->sp) & 0xFF));
                    emu->set_af_h(sum1 > 0x0F);
                    emu->set_af_c(sum2 > 0xFF);
                },
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 2,
                .operands = {
                    {
                        .r_16 = 2, // reg hl
                        .type = operand_type::r_16
                    },
                    {
                        .i_16 = u16(sp + std::bit_cast<i8>(n_plus_1.view())),
                        .type = operand_type::i_16
                    },
                }
            };
        } else if (opcode == 0xF9) { // ld sp, hl
            return {
                .handler = i::ld,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 1,
                .operands = {
                    {
                        .r_16 = 3, // reg sp
                        .type = operand_type::r_16
                    },
                    {
                        .i_16 = hl,
                        .type = operand_type::i_16
                    },
                }
            };
        } else if (opcode == 0x08) { // ld [a16], sp
            return {
                .handler = i::ld,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 3,
                .operands = {
                    {
                        .memory_address = le_combine(n_plus_1, n_plus_2), // reg sp
                        .type = operand_type::memory_address
                    },
                    {
                        .i_16 = sp,
                        .type = operand_type::i_16
                    },
                }
            };
        } else if ((opcode & 0xC3) == 0x03) { // inc r16
            return {
                .handler = i::inc,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 1,
                .operands = {
                    {
                        .r_16 = get_r16(opcode, 4),
                        .type = operand_type::r_16
                    }
                }
            };
        } else if ((opcode & 0xC7) == 0x04) { // inc r8
            return {
                .handler = i::inc,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 1,
                .operands = {
                    {
                        .r_8 = get_r8(opcode, 3),
                        .type = operand_type::r_8
                    }
                }
            };
        } else if (opcode == 0x07) {
            return {
                .handler = i::rlca,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 1,
                .operands = {
                    {
                        .r_8 = get_r8(opcode, 3),
                        .type = operand_type::r_8
                    }
                }
            };
        } else if (opcode == 0x0F) {
            return {
                .handler = i::rrca,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 1,
                .operands = {
                    {
                        .r_8 = get_r8(opcode, 3),
                        .type = operand_type::r_8
                    }
                }
            };
        } else if (opcode == 0x17) {
            return {
                .handler = i::rla,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 1,
                .operands = {
                    {
                        .r_8 = get_r8(opcode, 3),
                        .type = operand_type::r_8
                    }
                }
            };
        } else if (opcode == 0x1F) {
            return {
                .handler = i::rra,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 1,
                .operands = {
                    {
                        .r_8 = get_r8(opcode, 3),
                        .type = operand_type::r_8
                    }
                }
            };
        } else if (opcode == 0x27) {
            return {
                .handler = i::daa,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 1,
                .operands = {
                    {
                        .r_8 = get_r8(opcode, 3),
                        .type = operand_type::r_8
                    }
                }
            };
        } else if (opcode == 0x2F) {
            return {
                .handler = i::cpl,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 1,
                .operands = {
                    {
                        .r_8 = get_r8(opcode, 3),
                        .type = operand_type::r_8
                    }
                }
            };
        } else if (opcode == 0x37) {
            return {
                .handler = i::scf,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 1,
                .operands = {
                    {
                        .r_8 = get_r8(opcode, 3),
                        .type = operand_type::r_8
                    }
                }
            };
        } else if (opcode == 0x3F) {
            return {
                .handler = i::ccf,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 1,
                .operands = {
                    {
                        .r_8 = get_r8(opcode, 3),
                        .type = operand_type::r_8
                    }
                }
            };
        } else if (opcode >= 0b10000000 && opcode <= 0b10000111) {
            return {
                .handler = i::add,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 1,
                .operands = {
                    {
                        .r_8 = 7, // a
                        .type = operand_type::r_8
                    },
                    {
                        .r_8 = get_r8(opcode, 3),
                        .type = operand_type::r_8
                    }
                }
            };
        } else if (opcode >= 0b10001000 && opcode <= 0b10001111) {
            return {
                .handler = i::adc,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 1,
                .operands = {
                    {
                        .r_8 = 7, // a
                        .type = operand_type::r_8
                    },
                    {
                        .r_8 = get_r8(opcode, 3),
                        .type = operand_type::r_8
                    }
                }
            };
        } else if (opcode >= 0b10010000 && opcode <= 0b10010111) {
            return {
                .handler = i::sub,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 1,
                .operands = {
                    {
                        .r_8 = 7, // a
                        .type = operand_type::r_8
                    },
                    {
                        .r_8 = get_r8(opcode, 3),
                        .type = operand_type::r_8
                    }
                }
            };
        } else if (opcode >= 0b10011000 && opcode <= 0b10011111) {
            return {
                .handler = i::sbc,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 1,
                .operands = {
                    {
                        .r_8 = 7, // a
                        .type = operand_type::r_8
                    },
                    {
                        .r_8 = get_r8(opcode, 3),
                        .type = operand_type::r_8
                    }
                }
            };
        } else if (opcode >= 0b10100000 && opcode <= 0b10100111) {
            return {
                .handler = i::and_,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 1,
                .operands = {
                    {
                        .r_8 = 7, // a
                        .type = operand_type::r_8
                    },
                    {
                        .r_8 = get_r8(opcode, 3),
                        .type = operand_type::r_8
                    }
                }
            };
        } else if (opcode >= 0b10101000 && opcode <= 0b10101111) {
            return {
                .handler = i::xor_,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 1,
                .operands = {
                    {
                        .r_8 = 7, // a
                        .type = operand_type::r_8
                    },
                    {
                        .r_8 = get_r8(opcode, 3),
                        .type = operand_type::r_8
                    }
                }
            };
        } else if (opcode >= 0b10110000 && opcode <= 0b10110111) {
            return {
                .handler = i::or_,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 1,
                .operands = {
                    {
                        .r_8 = 7, // a
                        .type = operand_type::r_8
                    },
                    {
                        .r_8 = get_r8(opcode, 3),
                        .type = operand_type::r_8
                    }
                }
            };
        } else if (opcode >= 0b10111000 && opcode <= 0b10111111) {
            return {
                .handler = i::cp,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 1,
                .operands = {
                    {
                        .r_8 = 7, // a
                        .type = operand_type::r_8
                    },
                    {
                        .r_8 = get_r8(opcode, 3),
                        .type = operand_type::r_8
                    }
                }
            };
        } else if (opcode == 0xC6) { // add a, imm8
            return {
                .handler = i::add,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 2,
                .operands = {
                    {
                        .r_8 = 7, // a
                        .type = operand_type::r_8
                    },
                    {
                        .i_8 = n_plus_1,
                        .type = operand_type::i_8
                    }
                }
            };
        } else if (opcode == 0xCE) { // adc a, imm8
            return {
                .handler = i::adc,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 2,
                .operands = {
                    {
                        .r_8 = 7, // a
                        .type = operand_type::r_8
                    },
                    {
                        .i_8 = n_plus_1,
                        .type = operand_type::i_8
                    }
                }
            };
        } else if (opcode == 0xD6) { // sub a, imm8
            return {
                .handler = i::sub,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 2,
                .operands = {
                    {
                        .r_8 = 7, // a
                        .type = operand_type::r_8
                    },
                    {
                        .i_8 = n_plus_1,
                        .type = operand_type::i_8
                    }
                }
            };
        } else if (opcode == 0xDE) { // sbc a, imm8
            return {
                .handler = i::sbc,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 2,
                .operands = {
                    {
                        .r_8 = 7, // a
                        .type = operand_type::r_8
                    },
                    {
                        .i_8 = n_plus_1,
                        .type = operand_type::i_8
                    }
                }
            };
        } else if (opcode == 0xE6) { // and a, imm8
            return {
                .handler = i::and_,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 2,
                .operands = {
                    {
                        .r_8 = 7, // a
                        .type = operand_type::r_8
                    },
                    {
                        .i_8 = n_plus_1,
                        .type = operand_type::i_8
                    }
                }
            };
        } else if (opcode == 0xEE) { // xor a, imm8
            return {
                .handler = i::xor_,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 2,
                .operands = {
                    {
                        .r_8 = 7, // a
                        .type = operand_type::r_8
                    },
                    {
                        .i_8 = n_plus_1,
                        .type = operand_type::i_8
                    }
                }
            };
        } else if (opcode == 0xF6) { // or a, imm8
            return {
                .handler = i::or_,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 2,
                .operands = {
                    {
                        .r_8 = 7, // a
                        .type = operand_type::r_8
                    },
                    {
                        .i_8 = n_plus_1,
                        .type = operand_type::i_8
                    }
                }
            };
        } else if (opcode == 0xFE) { // cp a, imm8
            return {
                .handler = i::cp,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 2,
                .operands = {
                    {
                        .r_8 = 7, // a
                        .type = operand_type::r_8
                    },
                    {
                        .i_8 = n_plus_1,
                        .type = operand_type::i_8
                    }
                }
            };
        } else if ((opcode & 0xE7) == 0xC0) {
            return {
                .handler = i::ret,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 1,
                .operands = {
                    {
                        .condition = get_cond(opcode, 3),
                        .type = operand_type::condition
                    }
                }
            };
        } else if (opcode == 0xC9) {
            return {
                .handler = i::ret,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 1,
                .operands = {}
            };
        } else if (opcode == 0xD9) {
            return {
                .handler = i::reti,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 1,
                .operands = {}
            };
        } else if (opcode == 0xF3) {
            return {
                .handler = i::di,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 1,
                .operands = {}
            };
        } else if (opcode == 0xFB) {
            return {
                .handler = i::ei,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 1,
                .operands = {}
            };
        } else if (opcode == 0xC3) {
            // jp a16
            return {
                .handler = i::jp,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 3,
                .operands = {
                    {
                        .i_16 = le_combine(n_plus_1, n_plus_2),
                        .type = operand_type::i_16
                    }
                }
            };
        } else if ((opcode & 0xC7) == 0xC7) {
            return {
                .handler = i::rst,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 1,
                .operands = {
                    {
                        .i_8 = get_r16(opcode, 3), // not actually r16, but it works so yeah
                        .type = operand_type::i_8
                    }
                }
            };
        } else if ((opcode & 0xCF) == 0x09) {
            return {
                .handler = i::add,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 1,
                .operands = {
                    {
                        .r_16 = 2, // hl
                        .type = operand_type::r_16
                    },
                    {
                        .r_16 = get_r16(opcode, 3),
                        .type = operand_type::r_16
                    }
                }
            };
        } else if ((opcode & 0xC7) == 0x05) {
            return {
                .handler = i::dec,
                .cur_pc = pc,
                .opcode = opcode,
                .bytes = 1,
                .operands = {
                    {
                        .r_8 = get_r8(opcode, 3),
                        .type = operand_type::r_8
                    }
                }
            };
        }
        else {
            if (opcode == 0xDD) {
                spdlog::critical("execution of invalid memory (0xDD)");
            } else {
                spdlog::critical("unimplemented instruction at PC=0x{:04X}: 0x{:X}", pc, opcode);
            }
            spdlog::critical("total instructions decoded: {}", this->debug_stats.decoded_instructions);
            std::terminate();
        }
    }

    void emulator_t::logic() noexcept {
        using namespace toggle::cycle;
        u8 opcode = read8(pc);
        // Don't read unless we HAVE to
        // Prevents out-of-bounds memory access
        cycler<u8> n_plus_1([&]() -> u8 {
            return read8(pc + 1, no_cycle);
        });
        cycler<u8> n_plus_2([&]() -> u8 {
            return read8(pc + 2, no_cycle);
        });

        n_plus_1.emu = this;
        n_plus_2.emu = this;

        const instruction_t ins = decode(opcode, n_plus_1, n_plus_2);
        pc += ins.bytes;
        ins.handler(this, ins);

        ++this->debug_stats.decoded_instructions;
    }

    void emulator_t::init(std::vector<u8> &&bytes) noexcept {
        this->cartridge = std::move(bytes);

        u8 logo[] = {0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D, 0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99, 0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E};
        memcpy(wram + 4, logo, sizeof(logo));
    }

    void emulator_t::step(u32 n) noexcept {
        while (n-- != 0) {
            logic();

            incr_tima();
            if (clock % 16384 == 0 && !stop_mode) {
                ++div;
            }

            // Trigger em
            interrupts();

            handle_interrupts();

            if (lyc == ly) {
                stat |= (1 << 2);
                request_interrupt(int_stat);
            } else {
                stat &= ~(1 << 2);
            }
        }
    }

    bool emulator_t::is_cond(cond c) const noexcept {
        switch (c) {
            default:
                return false;
            case cond::carry:
                return af_c();
            case cond::notcarry:
                return !af_c();
            case cond::zero:
                return af_z();
            case cond::notzero:
                return !af_z();
        }
    }

    void emulator_t::ppu::tick() noexcept {
        // TODO: Implement PPU
    }
