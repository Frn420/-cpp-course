#ifndef COMPUTER_H
#define COMPUTER_H

#include <bits/stdc++.h>

using id_t = uint64_t;

template<typename... Ts>
struct Program {};

template<std::size_t Size, typename Type>
class Computer {
public:

    template<typename Program>
    static constexpr void boot() {
        static_assert(Size > 0);
    }
};

constexpr uint64_t number(char c) {
    if ('0' <= c && c <= '9') {
        return c - '0' + 1;
    } else if ('a' <= c && c <= 'z') {
        return c - 'a' + 11;
    } else if ('A' <= c && c <= 'Z') {
        return c - 'A' + 11;
    }

    //static_assert(false);
}

constexpr id_t Id(const char* word) {
    //static_assert(1 <= strlen(word) || strlen(word) <= 6);
    if (*word == '\0') {
        return 0;
    }
    return Id(word + 1) * 37 + number(word[0]);
}

template<auto Val>
struct Num {
    static constexpr auto value = Val;
};

template<typename Addr>
struct Mem {};

template<id_t Id>
struct Lea {};

template<id_t Id, typename Value>
struct D {};

template<typename Dst, typename Src>
struct Mov {};

template<typename Arg1, typename Arg2>
struct Add {};

template<typename Arg1, typename Arg2>
struct Sub {};

template<typename Arg>
struct Inc {};

template<typename Arg>
struct Dec {};

template<typename Arg1, typename Arg2>
struct And {};

template<typename Arg1, typename Arg2>
struct Or {};

template<typename Arg>
struct Not {};

template<typename Arg1, typename Arg2>
struct Cmp {};

template<typename Id>
struct Label {};

template<typename Label>
struct Jmp {};

template<typename Label>
struct Jz {};

template<typename Label>
struct Js {};

#endif // COMPUTER_H