#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <string>
#include <vector>

#include <fmt/core.h>

#include <event.hpp>
#include <request.hpp>

namespace tinyxml2 {
class XMLElement;
} // namespace tinyxml2

class GeneratorHeader;
class GeneratorCode;

class Interface {
  public:
    explicit Interface(const tinyxml2::XMLElement *xmlElement) noexcept;

  private:
    std::string m_name{};
    unsigned int m_version{1};
    std::string m_description{};
    std::vector<Request> m_requests{};
    std::vector<Event> m_events{};

    friend fmt::formatter<Interface>;
    friend GeneratorHeader;
    friend GeneratorCode;
};

template <>
class fmt::formatter<Interface> {
  public:
    static constexpr auto parse(fmt::format_parse_context &ctx) {
        return ctx.begin();
    }

    template <typename Context>
    static constexpr auto format(const Interface &interface, Context &ctx) {
        auto it = fmt::format_to(
            ctx.out(), "Interface: {}, version: {}, description: {}.\n",
            interface.m_name, interface.m_version, interface.m_description);

        if (!interface.m_requests.empty()) {
            it = fmt::format_to(it, "      Requests ({}):\n",
                                interface.m_requests.size());
        }
        for (const auto &request : interface.m_requests) {
            it = fmt::format_to(it, "    {}\n", request);
        }

        if (!interface.m_events.empty()) {
            it = fmt::format_to(it, "      Events ({}):\n",
                                interface.m_events.size());
        }
        for (const auto &event : interface.m_events) {
            it = fmt::format_to(it, "    {}\n", event);
        }

        return it;
    }
};
#endif
