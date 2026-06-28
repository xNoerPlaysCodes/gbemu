#pragma once

#include "types.hpp"

namespace bitutil {
    template<typename UintT>
    [[nodiscard]] inline constexpr UintT set(UintT byte, u8 position) noexcept {
        return byte | (1 << position);
    }

    template<typename UintT>
    [[nodiscard]] inline constexpr UintT clear(UintT byte, u8 position) noexcept {
        return byte & ~(1 << position);
    }

    template<typename UintT>
    [[nodiscard]] inline constexpr UintT change(UintT byte, u8 position, bool value) noexcept {
        if (value) {
            return set(byte, position);
        } else {
            return clear(byte, position);
        }
    }

    template<typename UintT>
    [[nodiscard]] inline constexpr UintT check(UintT byte, u8 position) noexcept {
        return byte & (1 << position);
    }

    template<typename UintT>
    [[nodiscard]] inline constexpr UintT toggle(UintT byte, u8 position) noexcept {
        return byte ^ (1 << position);
    }

    template<typename IntT>
    [[nodiscard]] inline constexpr IntT abs(IntT n) noexcept {
        return n < 0 ? -n : n;
    }

    template<typename UintT>
    [[nodiscard]] inline constexpr auto safe_sub(UintT a, UintT b) noexcept {
        return a > b ? a - b : b - a;
    }
}
