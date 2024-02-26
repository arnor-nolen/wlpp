#include <cstddef>
#include <cstdlib>
#include <fmt/format.h>
#include <span>
#include <tinyxml2.h>

#include <wlpp/generator_code.hpp>
#include <wlpp/generator_header.hpp>
#include <wlpp/generator_mode.hpp>
#include <wlpp/protocol.hpp>
#include <wlpp/util.hpp>

auto main(int argc, char *argv[]) -> int {

    auto args = std::span{argv, static_cast<size_t>(argc)};

    if (args.empty()) {
        fmt::print("Usage: <program_name> <xml_file> <generator_mode>.\n");
        return EXIT_FAILURE;
    }

    if (args.size() > 3) {
        fmt::print("Usage: {} <xml_file> <generator_mode>.\n", args[0]);
        return EXIT_FAILURE;
    }

    if (args.size() == 2) {
        fmt::print("Usage: {} <xml_file> <generator_mode>.\n", args[0]);
        return EXIT_FAILURE;
    }

    if (args.size() == 1) {
        fmt::print("wlpp version: 1.0.0.\n");
        fmt::print("Usage: {} <xml_file> <generator_mode>.\n", args[0]);
        return EXIT_FAILURE;
    }

    tinyxml2::XMLDocument doc{};
    if (doc.LoadFile(args[1]) != tinyxml2::XML_SUCCESS) {
        fmt::print("Error loading file: {}.\n", args[1]);
        return EXIT_FAILURE;
    }

    const auto *protocolPtr = doc.FirstChildElement("protocol");
    nullptrCheck(protocolPtr);

    const auto protocol = Protocol{protocolPtr};

    const auto genMode = strToGeneratorMode(args[2]);

    switch (genMode) {
    case GeneratorMode::Debug: {
        fmt::print("{}\n", protocol);
    } break;
    case GeneratorMode::Header: {
        const auto headerGen = GeneratorHeader{};
        protocol.generate(headerGen);
    } break;
    case GeneratorMode::Code: {
        const auto codeGen = GeneratorCode{};
        protocol.generate(codeGen);
    } break;
    default:
        halt("Unknown generator mode.");
    }

    return EXIT_SUCCESS;
}
