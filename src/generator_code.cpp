#include <wlpp/generator_code.hpp>

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
void GeneratorCode::dump(const Protocol &protocol) noexcept {
    auto capitalizedName = protocol.m_name;
    std::transform(capitalizedName.begin(), capitalizedName.end(),
                   capitalizedName.begin(), toupper);

    fmt::print("#include \"wayland-client-protocol.hpp\"\n\n");
    fmt::print("\n\n");

    fmt::print("/*{}\n*/\n\n", protocol.m_copyright);

    fmt::print("\n");

    fmt::print("\nnamespace {{\n");
    for (const auto &interface : protocol.m_interfaces) {
        fmt::print("extern const wl_interface s_native{}Interface;\n",
                   snakeToPascal(interface.m_name));
    }
    fmt::print("}} // namespace\n");

    for (const auto &interface : protocol.m_interfaces) {
        fmt::print("\nnamespace {{\n");

        const auto totalArgs = std::accumulate(
            interface.m_requests.cbegin(), interface.m_requests.cend(), 0,
            [](const auto &lhs, const auto &rhs) {
                return lhs + rhs.m_args.size();
            });
        fmt::print(R"(
constexpr std::array<const wl_interface *, {}u>
    s_native{}RequestsParamInterfaces = {{
)",
                   totalArgs, snakeToPascal(interface.m_name));
        for (const auto &request : interface.m_requests) {
            for (const auto &arg : request.m_args) {
                if (arg.m_type == ArgType::NewId && arg.m_interface) {
                    fmt::print("        &s_native{}Interface,\n",
                               snakeToPascal(*arg.m_interface));
                } else {
                    fmt::print("        nullptr,\n");
                }
            }
        }
        fmt::print("}};\n\n");

        fmt::print("constexpr auto s_max{}InterfaceVersion = {}u;\n",
                   snakeToPascal(interface.m_name), interface.m_version);

        if (!interface.m_requests.empty()) {
            fmt::print(R"(
constexpr std::array<wl_message, {}u> s_native{}Requests = {{{{)",
                       interface.m_requests.size(),
                       snakeToPascal(interface.m_name));

            size_t currentPos = 0u;
            for (const auto &request : interface.m_requests) {

                fmt::print(
                    R"(
    {{"{}", "{}", {}}},)",
                    request.m_name,
                    interface.m_name == "wl_registry" &&
                            request.m_name == "bind"
                        ? "usun"
                        : argsToWlArgString(request.m_args, request.m_since),
                    request.m_args.empty()
                        ? "nullptr"
                        : fmt::format(
                              "const_cast<const wl_interface "
                              "**>(&s_native{}RequestsParamInterfaces[{}])",
                              snakeToPascal(interface.m_name), currentPos));
                currentPos += request.m_args.size();
            }
            fmt::print(R"(
}}}};)");
        }

        if (!interface.m_events.empty()) {
            fmt::print(R"(
constexpr std::array<wl_message, {}u> s_native{}Events = {{{{)",
                       interface.m_events.size(),
                       snakeToPascal(interface.m_name));
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
constexpr wl_interface s_native{}Interface = {{
    "{}",
    s_max{}InterfaceVersion,
    {},
    {},
    {},
    {},
}};)",
            snakeToPascal(interface.m_name), interface.m_name,
            snakeToPascal(interface.m_name),
            interface.m_requests.empty()
                ? "0"
                : fmt::format("s_native{}Requests.size()",
                              snakeToPascal(interface.m_name)),
            interface.m_requests.empty()
                ? "nullptr"
                : fmt::format("s_native{}Requests.data()",
                              snakeToPascal(interface.m_name)),
            interface.m_events.empty()
                ? "0"
                : fmt::format("s_native{}Events.size()",
                              snakeToPascal(interface.m_name)),
            interface.m_events.empty()
                ? "nullptr"
                : fmt::format("s_native{}Events.data()",
                              snakeToPascal(interface.m_name)));

        fmt::print("\n}} // namespace\n\n");

        const auto interfaceNamePascal = snakeToPascal(interface.m_name);

        fmt::print("{}::{}(wl_proxy *nativeHandle) noexcept\n"
                   "    : m_nativeHandle{{nativeHandle}} {{}}\n\n",
                   interfaceNamePascal, interfaceNamePascal);

        for (size_t j = 0u; j < interface.m_requests.size(); ++j) {
            const auto &request = interface.m_requests[j];
            const auto requestNameCamel = snakeToCamel(request.m_name);

            const auto newIdIt =
                std::ranges::find_if(request.m_args, [](const auto &elem) {
                    return elem.m_type == ArgType::NewId;
                });

            const auto hasNewId = newIdIt != request.m_args.end();

            fmt::print("{} {}::{}(", hasNewId ? "auto" : "void",
                       interfaceNamePascal, requestNameCamel);

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
                    fmt::print(R"() const noexcept -> {} {{
	auto* object = wl_proxy_marshal_flags(m_nativeHandle.get(),
			 {}, &s_native{}Interface, wl_proxy_get_version(m_nativeHandle.get()), 0, nullptr)",
                               snakeToPascal(*newIdIt->m_interface), j,
                               snakeToPascal(interface.m_name));

                    for (const auto &arg : request.m_args) {
                        if (arg.m_type == ArgType::NewId) {
                            continue;
                        }

                        auto argNameCamel = snakeToCamel(arg.m_name);
                        if (argNameCamel == "class") {
                            argNameCamel = "className";
                        }

                        auto argTypeStr = argTypeToCppType(arg.m_type);
                        if (argTypeStr == "std::string") {
                            fmt::print(", {}.c_str()", argNameCamel);
                        } else {
                            fmt::print(", {}", argNameCamel);
                        }
                    }

                    fmt::print(R"();

	return {}{{object}};
}}

)",
                               snakeToPascal(*newIdIt->m_interface));

                } else {
                    // Wayland protocol wl_registry.bind() is an
                    // exception.
                    fmt::print(
                        R"(, const wl_interface *interface, unsigned int version) const noexcept -> void * {{
	return wl_proxy_marshal_flags(m_nativeHandle.get(),
			 {}, &s_native{}Interface, wl_proxy_get_version(m_nativeHandle.get()), 0, name, interface->name, version, nullptr);
}}

)",
                        j, snakeToPascal(interface.m_name));
                }
            } else {
                fmt::print(
                    R"() const noexcept {{
    wl_proxy_marshal_flags(m_nativeHandle.get(), {}, 
        nullptr, wl_proxy_get_version(m_nativeHandle.get()), 0)",
                    j);

                for (const auto &arg : request.m_args) {
                    auto argNameCamel = snakeToCamel(arg.m_name);
                    if (argNameCamel == "class") {
                        argNameCamel = "className";
                    }

                    auto argTypeStr = argTypeToCppType(arg.m_type);
                    if (argTypeStr == "std::string") {
                        fmt::print(", {}.c_str()", argNameCamel);
                    } else {
                        fmt::print(", {}", argNameCamel);
                    }
                }
                fmt::print(");\n}}\n\n");
            }
        }

        fmt::print("auto {}::getNativeHandle() const "
                   "noexcept -> wl_proxy * {{\n",
                   interfaceNamePascal);
        fmt::print("    return m_nativeHandle.get();\n");
        fmt::print("}};\n\n");
    };
}
