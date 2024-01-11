#include <cstddef>
#include <cstdlib>
#include <exception>
#include <fmt/format.h>
#include <span>
#include <tinyxml2.h>

void nullptrCheck(const void *ptr) {
    if (ptr == nullptr) {
        throw std::runtime_error("Error parsing file.");
    }
}

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

    const char *protocolName = protocolPtr->Attribute("name");
    nullptrCheck(protocolName);

    fmt::print("Protocol name: {}.\n", protocolName);

    const auto *copyrightPtr = protocolPtr->FirstChildElement("copyright");
    nullptrCheck(copyrightPtr);

    const char *copyrightText = copyrightPtr->GetText();
    nullptrCheck(copyrightText);

    fmt::print("Copyright: <omitted>.\n");

    const auto *interfacePtr = protocolPtr->FirstChildElement("interface");
    const char *interfaceName = nullptr;
    auto interfaceVersion = 1u;

    const tinyxml2::XMLElement *descPtr = nullptr;
    const char *descText = nullptr;

    int interfaceNum = 0;
    while (interfacePtr != nullptr) {

        interfaceName = interfacePtr->Attribute("name");
        nullptrCheck(interfaceName);

        interfacePtr->QueryUnsignedAttribute("version", &interfaceVersion);

        fmt::print("Interface ({}): {}, version {}.\n", interfaceNum,
                   interfaceName, interfaceVersion);

        // Description.
        descPtr = interfacePtr->FirstChildElement("description");
        nullptrCheck(descPtr);

        descText = descPtr->Attribute("summary");
        nullptrCheck(descText);
        fmt::print("  Summary: {}.\n", descText);

        // Requests.
        const tinyxml2::XMLElement *requestPtr = nullptr;
        const char *requestName = nullptr;
        auto requestSince = 1u;
        int requestNum = 0;

        requestPtr = interfacePtr->FirstChildElement("request");
        fmt::print("  Requests:\n");

        while (requestPtr != nullptr) {
            requestName = requestPtr->Attribute("name");
            nullptrCheck(requestName);

            requestPtr->QueryUnsignedAttribute("since", &requestSince);

            fmt::print("    Request ({}): {}, since {}.\n", requestNum,
                       requestName, requestSince);

            requestPtr = requestPtr->NextSiblingElement("request");
            ++requestNum;
        }

        // Events.
        const tinyxml2::XMLElement *eventPtr = nullptr;
        const char *eventName = nullptr;
        auto eventSince = 1u;
        int eventNum = 0;

        eventPtr = interfacePtr->FirstChildElement("event");
        fmt::print("  Events:\n");

        while (eventPtr != nullptr) {
            eventName = eventPtr->Attribute("name");
            nullptrCheck(eventName);

            eventPtr->QueryUnsignedAttribute("since", &eventSince);

            fmt::print("    Event ({}): {}, since {}.\n", eventNum, eventName,
                       eventSince);

            eventPtr = eventPtr->NextSiblingElement("event");
            ++eventNum;
        }

        interfacePtr = interfacePtr->NextSiblingElement("interface");
        ++interfaceNum;
    }

    return EXIT_SUCCESS;
} catch (const std::exception &e) {
    fmt::print("Exception: {}\n", e.what());

    return EXIT_FAILURE;
}
