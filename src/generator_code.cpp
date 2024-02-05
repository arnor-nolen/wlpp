#include <generator_code.hpp>

#include <algorithm>
#include <cassert>
#include <cctype>
#include <ranges>

#include <fmt/format.h>

#include <protocol.hpp>
#include <util.hpp>

void GeneratorCode::dump(const Protocol &protocol) noexcept {
    auto capitalizedName = protocol.m_name;
    std::transform(capitalizedName.begin(), capitalizedName.end(),
                   capitalizedName.begin(), toupper);

    fmt::print("#include \"wayland-client-protocol.hpp\"\n\n");
    fmt::print("\n\n");

    fmt::print("/*{}\n*/\n\n", protocol.m_copyright);

    fmt::print("\n");

    for (const auto &interface : protocol.m_interfaces) {
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
			 {}, &s_nativeInterface, wl_proxy_get_version(m_nativeHandle.get()), 0, nullptr)",
                               snakeToPascal(*newIdIt->m_interface), j);

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
                    fmt::print(") const noexcept -> void * {{}}\n\n");
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
