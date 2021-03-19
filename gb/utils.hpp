#pragma once
#include "types.hpp"
namespace gb::utils {
    template <typename T>
    [[nodiscard]] static constexpr auto to_signed(T val) noexcept {
        return static_cast<std::make_signed_t<T>>(val);
    }

    constexpr auto word_unpack(word_t value) noexcept {
        struct Pair {
            byte_t lo;
            byte_t hi;
        };
        return Pair { static_cast<byte_t>(value), static_cast<byte_t>(value >> 8) };
    }

    constexpr auto word_pack(byte_t lo, byte_t hi) noexcept {
        return static_cast<word_t>(lo | (hi << 8));
    }

    [[nodiscard]] constexpr bool match(byte_t value, char const(&pattern)[9]) {
        std::uint8_t mask = 0x80;
        for (auto i = pattern; auto const c = *i; ++i) {
            if (c == '0') {
                if (value & mask) {
                    return false;
                }
            } else if (c == '1') {
                if (!(value & mask)) {
                    return false;
                }
            } else if (c < 'a' || c > 'z') {
                throw "Invalid mask character!";
            }
            mask >>= 1;
        }
        return true;
    }

    [[nodiscard]] constexpr bool match(byte_t value, int exact) noexcept {
        return value == exact;
    }

    template <typename...T>
    [[nodiscard]] constexpr bool match(byte_t value, T&&...exact) {
        return (match(value, std::forward<T>(exact)) || ...);
    }
}
