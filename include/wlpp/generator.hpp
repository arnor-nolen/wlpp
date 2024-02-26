#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include <concepts>
#include <utility>

class Protocol;

template <typename T>
concept Generator = requires(const Protocol &protocol) {
    { std::declval<const T>().dump(protocol) } noexcept -> std::same_as<void>;
};
#endif
