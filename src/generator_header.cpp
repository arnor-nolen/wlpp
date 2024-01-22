#include <generator_header.hpp>

#include <algorithm>
#include <cctype>

#include <fmt/format.h>

#include <protocol.hpp>
#include <util.hpp>

void GeneratorHeader::dump(const Protocol &protocol) noexcept {
    auto capitalizedName = protocol.m_name;
    std::transform(capitalizedName.begin(), capitalizedName.end(),
                   capitalizedName.begin(), toupper);
    fmt::print("#ifndef {}_CLIENT_PROTOCOL_H\n#define {}_CLIENT_PROTOCOL_H\n\n",
               capitalizedName, capitalizedName);

    fmt::print("#include <string>\n\n");

    fmt::print("/*{}\n*/\n\n", protocol.m_copyright);

    for (const auto &interface : protocol.m_interfaces) {
        const auto interfaceNamePascal = snakeToPascal(interface.m_name);

        fmt::print("class {} {{\n  public:\n", interfaceNamePascal);
        fmt::print("    explicit {}() noexcept;\n", interfaceNamePascal);
        fmt::print("    ~{}();\n\n", interfaceNamePascal);

        fmt::print("    {}(const {} &) = delete;\n", interfaceNamePascal,
                   interfaceNamePascal);
        fmt::print("    {}({} &&) noexcept = delete;\n\n", interfaceNamePascal,
                   interfaceNamePascal);

        fmt::print("    auto operator=(const {} &) -> {} & = delete;\n",
                   interfaceNamePascal, interfaceNamePascal);
        fmt::print("    auto operator=({} &&) noexcept -> {} & = delete;\n",
                   interfaceNamePascal, interfaceNamePascal);

        if (!interface.m_requests.empty()) {
            fmt::print("\n");
        }

        for (const auto &request : interface.m_requests) {
            const auto requestNamePascal = snakeToCamel(request.m_name);

            fmt::print("    void {}(", requestNamePascal);

            for (size_t i = 0u; i < request.m_args.size(); ++i) {
                if (i != 0u) {
                    fmt::print(", ");
                }

                const auto argNamePascal =
                    snakeToCamel(request.m_args[i].m_name);

                fmt::print("{} {}", argTypeToCppType(request.m_args[i].m_type),
                           argNamePascal);
            }

            fmt::print(") noexcept;\n");
        }

        fmt::print("}};\n\n");
    };

    fmt::print("#endif\n");
}
