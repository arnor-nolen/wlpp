#include <cstddef>
#include <cstdlib>
#include <fmt/format.h>
#include <span>

auto main(int argc, char *argv[]) -> int {

    auto args = std::span{argv, static_cast<size_t>(argc)};

    if (args.empty()) {
        fmt::print("Usage: <program_name> <your_name>.\n");
        return EXIT_FAILURE;
    }

    if (args.size() > 2) {
        fmt::print("Usage: {} <your_name>.\n", args[0]);
        return EXIT_FAILURE;
    }

    if (args.size() == 1) {
        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}
