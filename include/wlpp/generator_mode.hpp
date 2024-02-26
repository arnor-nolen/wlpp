#ifndef GENERATOR_MODE_HPP
#define GENERATOR_MODE_HPP

#include <string>
#include <string_view>

#include <wlpp/util.hpp>

enum class GeneratorMode {
    Debug,
    Header,
    Code,
};

[[nodiscard]]
inline auto generatorModeToStr(GeneratorMode mode) -> std::string {
    switch (mode) {
    case GeneratorMode::Debug:
        return "debug";
    case GeneratorMode::Header:
        return "client-header";
    case GeneratorMode::Code:
        return "private-code";
    default:
        halt("Unknown generator mode.");
    }
}

[[nodiscard]]
inline auto strToGeneratorMode(std::string_view str) -> GeneratorMode {
    if (str == "debug") {
        return GeneratorMode::Debug;
    }

    if (str == "client-header") {
        return GeneratorMode::Header;
    }

    if (str == "private-code") {
        return GeneratorMode::Code;
    }

    halt("Unknown generator mode.");
}
#endif
