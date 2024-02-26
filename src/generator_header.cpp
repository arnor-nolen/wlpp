#include <wlpp/generator_header.hpp>

#include <algorithm>
#include <cassert>
#include <cctype>
#include <numeric>
#include <ranges>

#include <fmt/format.h>

#include <wlpp/protocol.hpp>
#include <wlpp/util.hpp>

namespace {

auto argsToWlArgString(std::ranges::input_range auto range,
                       unsigned int sinceVersion) -> std::string {
    auto wlArgString = std::string{};

    if (sinceVersion > 1) {
        wlArgString.append(std::to_string(sinceVersion));
    }

    for (const auto &elem : range) {
        wlArgString.append(elem.toWlString());
    }

    return wlArgString;
}

} // namespace

void GeneratorHeader::dump(const Protocol &protocol) noexcept {
    auto capitalizedName = protocol.m_name;
    std::transform(capitalizedName.begin(), capitalizedName.end(),
                   capitalizedName.begin(), toupper);
    fmt::print(
        "#ifndef {}_CLIENT_PROTOCOL_HPP\n#define {}_CLIENT_PROTOCOL_HPP\n\n",
        capitalizedName, capitalizedName);

    fmt::print("#include <array>\n#include <memory>\n#include <string>\n\n");
    fmt::print("#include <wayland-client-core.h>\n\n");

    fmt::print("/*{}\n*/\n\n", protocol.m_copyright);

    for (const auto &interface : protocol.m_interfaces) {
        fmt::print("class {};\n", snakeToPascal(interface.m_name));
    }

    fmt::print("\n");

    for (const auto &interface : protocol.m_interfaces) {
        const auto interfaceNamePascal = snakeToPascal(interface.m_name);

        fmt::print("class {} {{\n  public:\n", interfaceNamePascal);
        fmt::print("    explicit {}(wl_proxy *nativeHandle) noexcept;\n",
                   interfaceNamePascal);
        // fmt::print("    ~{}();\n\n", interfaceNamePascal);
        //
        // fmt::print("    {}(const {} &) = delete;\n", interfaceNamePascal,
        //            interfaceNamePascal);
        // fmt::print("    {}({} &&) noexcept = delete;\n\n",
        // interfaceNamePascal,
        //            interfaceNamePascal);
        //
        // fmt::print("    auto operator=(const {} &) -> {} & = delete;\n",
        //            interfaceNamePascal, interfaceNamePascal);
        // fmt::print("    auto operator=({} &&) noexcept -> {} & = delete;\n",
        //            interfaceNamePascal, interfaceNamePascal);

        if (!interface.m_requests.empty()) {
            fmt::print("\n");
        }

        for (const auto &request : interface.m_requests) {
            const auto requestNameCamel = snakeToCamel(request.m_name);

            const auto newIdIt =
                std::ranges::find_if(request.m_args, [](const auto &elem) {
                    return elem.m_type == ArgType::NewId;
                });

            const auto hasNewId = newIdIt != request.m_args.end();

            if (hasNewId) {
                fmt::print("    [[nodiscard]]\n");
            }

            fmt::print("    {} {}(", hasNewId ? "auto" : "void",
                       requestNameCamel);

            for (size_t i = 0u; i < request.m_args.size(); ++i) {
                const auto &arg = request.m_args[i];
                auto argNameCamel = snakeToCamel(arg.m_name);
                if (argNameCamel == "class") {
                    argNameCamel = "className";
                }

                if (arg.m_type == ArgType::NewId) {
                    continue;
                }

                if (i > static_cast<size_t>(hasNewId)) {
                    fmt::print(", ");
                }

                if (arg.m_type == ArgType::Object) {
                    // TODO: Check if null is allowed. If not, use
                    // references.
                    assert(arg.m_interface);
                    fmt::print("{} *{}", snakeToPascal(*arg.m_interface),
                               argNameCamel);
                } else {
                    fmt::print("{} {}", argTypeToCppType(arg.m_type),
                               argNameCamel);
                }
            }

            if (hasNewId) {
                if (newIdIt->m_interface) {
                    fmt::print(") const noexcept -> {};\n",
                               snakeToPascal(*newIdIt->m_interface));
                } else {
                    // Wayland protocol wl_registry.bind() is an exception.
                    fmt::print(", const wl_interface *interface, unsigned int "
                               "version) const noexcept -> void *;\n");
                }
            } else {
                fmt::print(") const noexcept;\n");
            }
        }

        fmt::print("\n    [[nodiscard]]\n    auto getNativeHandle() const "
                   "noexcept -> wl_proxy *;\n");

        fmt::print("\n    constexpr static auto s_maxInterfaceVersion = {}u;\n",
                   interface.m_version);

        if (!interface.m_requests.empty()) {
            const auto totalArgs = std::accumulate(
                interface.m_requests.cbegin(), interface.m_requests.cend(), 0,
                [](const auto &lhs, const auto &rhs) {
                    return lhs + rhs.m_args.size();
                });
            fmt::print(R"(
    constexpr static std::array<wl_interface *, {}u> s_nativeRequestsParamInterfaces = {{{{
)",
                       totalArgs);
            for (const auto &request : interface.m_requests) {
                for (const auto &arg : request.m_args) {
                    if (arg.m_type == ArgType::NewId && arg.m_interface) {
                        fmt::print("        {}::s_nativeInterface,\n",
                                   snakeToPascal(*arg.m_interface));
                    } else {
                        fmt::print("        nullptr,\n");
                    }
                }
            }
            fmt::print(R"(}}}};)");

            fmt::print(R"(
    constexpr static std::array<wl_message, {}u> s_nativeRequests = {{{{)",
                       interface.m_requests.size());

            size_t currentPos = 0u;
            for (const auto &request : interface.m_requests) {

                fmt::print(
                    R"(
        {{"{}", "{}", const_cast<const wl_interface **>(&s_nativeRequestsParamInterfaces[{}])}},)",
                    request.m_name,
                    interface.m_name == "wl_registry" &&
                            request.m_name == "bind"
                        ? "usun"
                        : argsToWlArgString(request.m_args, request.m_since),
                    currentPos);
                currentPos += request.m_args.size();
            }
            fmt::print(R"(
    }}}};)");
        }

        if (!interface.m_events.empty()) {
            fmt::print(R"(
    constexpr static std::array<wl_message, {}u> s_nativeEvents = {{{{)",
                       interface.m_events.size());
            for (const auto &event : interface.m_events) {
                fmt::print(R"(
        {{"{}", "{}", nullptr}},)",
                           event.m_name,
                           argsToWlArgString(event.m_args, event.m_since));
            }
            fmt::print(R"(
    }}}};)");
        }

        fmt::print(
            R"(
    constexpr static wl_interface s_nativeInterface = {{
        "{}",
        s_maxInterfaceVersion,
        {},
        {},
        {},
        {},
    }};)",
            interface.m_name,
            interface.m_requests.empty() ? "0" : "s_nativeRequests.size()",
            interface.m_requests.empty() ? "nullptr"
                                         : "s_nativeRequests.data()",
            interface.m_events.empty() ? "0" : "s_nativeEvents.size()",
            interface.m_events.empty() ? "nullptr" : "s_nativeEvents.data()");

        fmt::print("\n\n  private:\n    std::unique_ptr<wl_proxy> "
                   "m_nativeHandle;\n");
        fmt::print("}};\n\n");
    };

    fmt::print("#endif\n");
}
