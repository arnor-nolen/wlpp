#ifndef EVENT_HPP
#define EVENT_HPP

#include <string>

#include <fmt/core.h>

namespace tinyxml2 {
class XMLElement;
} // namespace tinyxml2

class GeneratorHeader;

class Event {
  public:
    explicit Event(const tinyxml2::XMLElement *xmlElement) noexcept;

  private:
    std::string m_name{};
    unsigned int m_since{1};
    std::string m_description{};
    // args

    friend fmt::formatter<Event>;
    friend GeneratorHeader;
};

template <>
class fmt::formatter<Event> {
  public:
    static constexpr auto parse(fmt::format_parse_context &ctx) {
        return ctx.begin();
    }

    template <typename Context>
    static constexpr auto format(const Event &event, Context &ctx) {
        return fmt::format_to(ctx.out(),
                              "    Event: {}, since: {}, description: {}.",
                              event.m_name, event.m_since, event.m_description);
    }
};
#endif
