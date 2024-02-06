#include <generator_header.hpp>

#include <algorithm>
#include <cassert>
#include <cctype>
#include <ranges>

#include <fmt/format.h>

#include <protocol.hpp>
#include <util.hpp>

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
                    fmt::print(") const noexcept -> void *;\n");
                }
            } else {
                fmt::print(") const noexcept;\n");
            }
        }

        fmt::print("\n    [[nodiscard]]\n    auto getNativeHandle() const "
                   "noexcept -> wl_proxy *;\n");

        fmt::print(R"(
    constexpr static std::array<wl_message, {}> s_nativeRequests = {{{{)",
                   interface.m_requests.size());
        for (const auto &request : interface.m_requests) {
            fmt::print(R"(
        {{"{}", "", nullptr}},)",
                       request.m_name);
        }
        fmt::print(R"(
    }}}};)");

        fmt::print(R"(
    constexpr static std::array<wl_message, {}> s_nativeEvents = {{{{)",
                   interface.m_events.size());
        for (const auto &event : interface.m_events) {
            fmt::print(R"(
        {{"{}", "", nullptr}},)",
                       event.m_name);
        }
        fmt::print(R"(
    }}}};)");

        fmt::print(R"(
    constexpr static wl_interface s_nativeInterface = {{
        "{}", {}, s_nativeRequests.size(), s_nativeRequests.data(), s_nativeEvents.size(), s_nativeEvents.data(),
    }};)",
                   interface.m_name, interface.m_version);

        fmt::print("\n  private:\n    std::unique_ptr<wl_proxy> "
                   "m_nativeHandle;\n");
        fmt::print("}};\n\n");
    };

    fmt::print("#endif\n");
}
