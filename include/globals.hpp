#pragma once

#include "types.hpp"
#include <cstddef>
constexpr size_t fb_x = 160;
constexpr size_t fb_y = 144;

extern size_t scale;

extern u8 g_joypad_state;
extern i32 g_rl_gamepad;

void poll_joypad_state(u8 *joypad_state);
