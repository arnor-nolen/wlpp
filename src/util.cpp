#include <util.hpp>

#include <cassert>

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

auto snakeToPascal(std::string_view snakeCase) -> std::string {
    assert(!snakeCase.empty());

    auto result = snakeToCamel(snakeCase);

    // NOLINTNEXTLINE(bugprone-narrowing-conversions, cppcoreguidelines-narrowing-conversions)
    result[0] = toupper(result[0]);

    return result;
}

auto snakeToCamel(std::string_view snakeCase) -> std::string {
    assert(!snakeCase.empty());

    std::string result{};

    bool convertNext = false;

    for (const char &ch : snakeCase) {
        if (convertNext) {
            // NOLINTNEXTLINE(bugprone-narrowing-conversions, cppcoreguidelines-narrowing-conversions)
            result += toupper(ch);
            convertNext = false;
        } else if (ch == '_') {
            convertNext = true;
        } else {
            result += ch;
        }
    }

    return result;
}
