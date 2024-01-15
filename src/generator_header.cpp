#include <generator_header.hpp>

#include <algorithm>
#include <cctype>

#include <fmt/format.h>

#include <protocol.hpp>

void GeneratorHeader::dump(const Protocol &protocol) noexcept {
    auto capitalizedName = protocol.m_name;
    std::transform(capitalizedName.begin(), capitalizedName.end(),
                   capitalizedName.begin(), toupper);
    fmt::print("#ifndef {}_CLIENT_PROTOCOL_H\n#define {}_CLIENT_PROTOCOL_H\n\n",
               capitalizedName, capitalizedName);

    fmt::print("/*{}\n*/\n\n", protocol.m_copyright);

    for (const auto &interface : protocol.m_interfaces) {
        fmt::print("class {} {{\n", interface.m_name);

        for (const auto &request : interface.m_requests) {
            fmt::print("    void {}() noexcept;\n", request.m_name);
        }

        fmt::print("}};\n\n");
    };

    fmt::print("#endif\n");
}
