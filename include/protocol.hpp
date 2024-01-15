#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include <string>
#include <vector>

#include <fmt/core.h>

#include <generator.hpp>
#include <interface.hpp>

namespace tinyxml2 {
class XMLElement;
} // namespace tinyxml2

class GeneratorHeader;

class Protocol {
  public:
    explicit Protocol(const tinyxml2::XMLElement *xmlElement) noexcept;

    void generate(const Generator auto &generator) const noexcept {
        generator.dump(*this);
    }

  private:
    std::string m_name{};
    std::string m_copyright{};
    std::vector<Interface> m_interfaces{};

    friend fmt::formatter<Protocol>;
    friend GeneratorHeader;
};

template <>
class fmt::formatter<Protocol> {
  public:
    static constexpr auto parse(fmt::format_parse_context &ctx) {
        return ctx.begin();
    }

    template <typename Context>
    static constexpr auto format(const Protocol &protocol, Context &ctx) {
        auto it =
            fmt::format_to(ctx.out(), "Protocol name: {}.\n  Copyright: {}\n",
                           protocol.m_name, protocol.m_copyright);

        if (!protocol.m_interfaces.empty()) {
            it = fmt::format_to(it, "  Interfaces ({}):\n",
                                protocol.m_interfaces.size());
        }
        for (const auto &interface : protocol.m_interfaces) {
            it = fmt::format_to(it, "    {}\n", interface);
        }

        return it;
    }
};
#endif
