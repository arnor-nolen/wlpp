#include <wlpp/arg.hpp>

#include <tinyxml2.h>

#include <wlpp/util.hpp>

auto strToArgType(std::string_view typeStr) -> ArgType {
    if (typeStr == "uint") {
        return ArgType::UnsignedInt;
    }

    if (typeStr == "int") {
        return ArgType::Int;
    }

    if (typeStr == "new_id") {
        return ArgType::NewId;
    }

    if (typeStr == "fd") {
        return ArgType::FileDescriptor;
    }

    if (typeStr == "string") {
        return ArgType::String;
    }

    if (typeStr == "object") {
        return ArgType::Object;
    }

    if (typeStr == "fixed") {
        return ArgType::Fixed;
    }

    if (typeStr == "array") {
        return ArgType::Array;
    }

    halt("Unknown arg type.");
}

auto argTypeToStr(ArgType argType) -> std::string {
    switch (argType) {
    case ArgType::UnsignedInt:
        return "uint";
    case ArgType::Int:
        return "int";
    case ArgType::NewId:
        return "new_id";
    case ArgType::FileDescriptor:
        return "fd";
    case ArgType::String:
        return "string";
    case ArgType::Object:
        return "object";
    case ArgType::Fixed:
        return "double";
    case ArgType::Array:
        return "array";
    default:
        halt("Unknown ArgType.");
    }
}

auto argTypeToCppType(ArgType argType) -> std::string {
    switch (argType) {
    case ArgType::UnsignedInt:
        return "unsigned int";
    case ArgType::Int:
        return "int";
    case ArgType::NewId:
        return "new_id";
    case ArgType::FileDescriptor:
        return "int";
    case ArgType::String:
        return "std::string";
    case ArgType::Object:
        return "object";
    case ArgType::Fixed:
        return "double";
    case ArgType::Array:
        return "array";
    default:
        halt("Unknown ArgType.");
    }
}

auto argTypeToChar(ArgType argType) -> char {
    switch (argType) {
    case ArgType::UnsignedInt:
        return 'u';
    case ArgType::Int:
        return 'i';
    case ArgType::NewId:
        return 'n';
    case ArgType::FileDescriptor:
        return 'h';
    case ArgType::String:
        return 's';
    case ArgType::Object:
        return 'o';
    case ArgType::Fixed:
        return 'f';
    case ArgType::Array:
        return 'a';
    default:
        halt("Unknown ArgType.");
    }
}

Arg::Arg(const tinyxml2::XMLElement *xmlElement) noexcept {
    const char *argName = xmlElement->Attribute("name");
    nullptrCheck(argName);
    m_name = argName;

    const char *argTypeStr = xmlElement->Attribute("type");
    nullptrCheck(argTypeStr);
    m_type = strToArgType(argTypeStr);

    const char *argSummary = xmlElement->Attribute("summary");
    if (argSummary != nullptr) {
        m_summary = argSummary;
    }

    const char *argInterface = xmlElement->Attribute("interface");
    if (argInterface != nullptr) {
        m_interface = argInterface;
    }

    xmlElement->QueryBoolAttribute("allow-null", &m_allowNull);
}

auto Arg::toWlString() const noexcept -> std::string {
    auto str = std::string{};

    if (m_allowNull) {
        str.push_back('?');
    }

    str.push_back(argTypeToChar(m_type));

    return str;
}
