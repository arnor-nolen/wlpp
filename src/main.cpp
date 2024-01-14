#include <cstddef>
#include <cstdlib>
#include <fmt/format.h>
#include <span>
#include <tinyxml2.h>

#include <protocol.hpp>
#include <util.hpp>

auto main(int argc, char *argv[]) -> int try {

    auto args = std::span{argv, static_cast<size_t>(argc)};

    if (args.empty()) {
        fmt::print("Usage: <program_name> <xml_file>.\n");
        return EXIT_FAILURE;
    }

    if (args.size() > 2) {
        fmt::print("Usage: {} <xml_file>.\n", args[0]);
        return EXIT_FAILURE;
    }

    if (args.size() == 1) {
        fmt::print("wlpp version: 1.0.0.\n");
        fmt::print("Usage: {} <xml_file>.\n", args[0]);
        return EXIT_FAILURE;
    }

    tinyxml2::XMLDocument doc{};
    if (doc.LoadFile(args[1]) != tinyxml2::XML_SUCCESS) {
        fmt::print("Error loading file: {}.\n", args[1]);
        return EXIT_FAILURE;
    }

    fmt::print("Loaded file: {}.\n", args[1]);

    const auto *protocolPtr = doc.FirstChildElement("protocol");
    nullptrCheck(protocolPtr);

    const auto protocol = Protocol{protocolPtr};
    fmt::print("{}\n", protocol);

    return EXIT_SUCCESS;
} catch (const std::exception &e) {
    fmt::print("Exception: {}\n", e.what());

    return EXIT_FAILURE;
}
