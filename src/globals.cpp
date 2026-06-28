#include "types.hpp"
#include "globals.hpp"
#include <raylib.h>

void poll_joypad_state(u8 *joypad_state) {
    u8 state = 0b00111111;
    bool dpad = false;
    bool stsel = false;
    if (g_rl_gamepad != -1) {
        for (i32 i = GAMEPAD_BUTTON_LEFT_FACE_UP; i <= GAMEPAD_BUTTON_LEFT_FACE_LEFT; ++i) {
            if (IsGamepadButtonDown(g_rl_gamepad, i)) {
                state |= 1 << 4;
                if (i == GAMEPAD_BUTTON_LEFT_FACE_UP) {
                    state &= ~(1 << 2);
                } else if (i == GAMEPAD_BUTTON_LEFT_FACE_DOWN) {
                    state &= ~(1 << 3);
                } else if (i == GAMEPAD_BUTTON_LEFT_FACE_LEFT) {
                    state &= ~(1 << 1);
                } else if (i == GAMEPAD_BUTTON_LEFT_FACE_RIGHT) {
                    state &= ~(1 << 0);
                }
                dpad = true;
            }
        }

        
        if (IsGamepadButtonDown(g_rl_gamepad, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
            state &= ~(1 << 0);
            stsel = true;
        } else if (IsGamepadButtonDown(g_rl_gamepad, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
            state &= ~(1 << 1);
            stsel = true;
        } else if (IsGamepadButtonDown(g_rl_gamepad, GAMEPAD_BUTTON_MIDDLE_LEFT)) {
            state &= ~(1 << 2);
            stsel = true;
        } else if (IsGamepadButtonDown(g_rl_gamepad, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
            state &= ~(1 << 3);
            stsel = true;
        }
    }

    if (!stsel && !dpad) {
        state |= 1 << 0;
        state |= 1 << 1;
        state |= 1 << 2;
        state |= 1 << 3;
    }

    if (stsel) {
        state &= ~(1 << 5);
    }

    if (dpad) {
        state &= ~(1 << 4);
    }

    *joypad_state = state;
}

size_t scale = 10;

u8 g_joypad_state = 0;
i32 g_rl_gamepad = -1;
