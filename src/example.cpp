#include <example.hpp>

#include <fmt/format.h>

Example::Example(std::string_view text) { fmt::print("Hello, {}!\n", text); }
