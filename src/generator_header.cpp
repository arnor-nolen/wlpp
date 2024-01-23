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
    fmt::print("#ifndef {}_CLIENT_PROTOCOL_H\n#define {}_CLIENT_PROTOCOL_H\n\n",
               capitalizedName, capitalizedName);

    fmt::print("#include <string>\n#include <memory>\n\n");

    fmt::print("/*{}\n*/\n\n", protocol.m_copyright);

    for (const auto &interface : protocol.m_interfaces) {
        const auto interfaceNamePascal = snakeToPascal(interface.m_name);

        fmt::print("class {} {{\n  public:\n", interfaceNamePascal);
        fmt::print("    explicit {}({} *nativeHandle) noexcept;\n",
                   interfaceNamePascal, interface.m_name);
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

            fmt::print("    {} {}(", hasNewId ? "auto" : "void",
                       requestNameCamel);

            for (size_t i = 0u; i < request.m_args.size(); ++i) {
                const auto &arg = request.m_args[i];
                const auto argNameCamel = snakeToCamel(arg.m_name);

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
                    fmt::print("{} *{}", snakeToCamel(*arg.m_interface),
                               argNameCamel);
                } else {
                    fmt::print("{} {}", argTypeToCppType(arg.m_type),
                               argNameCamel);
                }
            }

            if (hasNewId) {
                if (newIdIt->m_interface) {
                    fmt::print(") const noexcept -> {} *;\n",
                               snakeToPascal(*newIdIt->m_interface));
                } else {
                    // Wayland protocol wl_registry.bind() is an exception.
                    fmt::print(") const noexcept -> void *;\n");
                }
            } else {
                fmt::print(") const noexcept;\n");
            }
        }

        fmt::print("\n    auto getNativeHandle() const noexcept -> {} *;\n",
                   interface.m_name);

        fmt::print("\n  private:\n    std::unique_ptr<{}> m_nativeHandle;\n",
                   interface.m_name);
        fmt::print("}};\n\n");
    };

    fmt::print("#endif\n");
}
