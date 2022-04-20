#pragma once
#include <array>
#include <bit>
#include <cstdint>
#include <cstdio>
#include <type_traits>
#include <utility>

#define gb_func constexpr auto
#define gb_rep($n, $i, ...) []<auto... $i>(std::index_sequence<$i...>){__VA_ARGS__}(std::make_index_sequence<$n>())
#define gb_flag_ops($n)                                                                                   \
    gb_func inline operator~($n lhs) noexcept->$n { return ($n)(~(unsigned)lhs); }                        \
    gb_func inline operator&($n lhs, $n rhs) noexcept->$n { return ($n)((unsigned)lhs & (unsigned)rhs); } \
    gb_func inline operator^($n lhs, $n rhs) noexcept->$n { return ($n)((unsigned)lhs ^ (unsigned)rhs); } \
    gb_func inline operator|($n lhs, $n rhs) noexcept->$n { return ($n)((unsigned)lhs | (unsigned)rhs); }

namespace gb {
    using byte_t = std::uint8_t;
    using sbyte_t = std::int8_t;
    using word_t = std::uint16_t;
    using pair_t = struct pair_t { byte_t lo = 0, hi = 0; };
    using sword_t = std::int16_t;

    struct CPU;

    gb_func inline word_unpack(word_t value) noexcept->pair_t { return pair_t{(byte_t)(value), (byte_t)(value >> 8)}; }

    gb_func inline word_pack(byte_t lo, byte_t hi) noexcept->word_t { return (word_t)(lo | (hi << 8)); }

    gb_func inline one_of(auto value, auto... exact) -> bool { return ((value == exact) || ...); }

    gb_func inline bit_match(uint8_t value, char const (&pattern)[9])->bool {
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
}
