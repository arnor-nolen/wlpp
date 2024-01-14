#include <util.hpp>

#include <fmt/core.h>

void nullptrCheck(const void *ptr) {
    if (ptr == nullptr) {
        halt("Error parsing file.");
    }
}

[[noreturn]]
void halt(std::string_view message) {
    fmt::print("{}\n", message);
    std::terminate();
}
