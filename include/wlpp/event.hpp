#ifndef EVENT_HPP
#define EVENT_HPP

#include <string>
#include <vector>

#include <fmt/core.h>
#include <tinyxml2.h>

#include <wlpp/arg.hpp>

class GeneratorHeader;
class GeneratorCode;

class Event {
  public:
    explicit Event(const tinyxml2::XMLElement *xmlElement) noexcept;

  private:
    std::string m_name{};
    unsigned int m_since{1};
    std::string m_description{};
    std::vector<Arg> m_args{};

    friend fmt::formatter<Event>;
    friend GeneratorHeader;
    friend GeneratorCode;
};

template <>
class fmt::formatter<Event> {
  public:
    static constexpr auto parse(fmt::format_parse_context &ctx) {
        return ctx.begin();
    }

    template <typename Context>
    static constexpr auto format(const Event &event, Context &ctx) {
        auto it = fmt::format_to(
            ctx.out(), "    Event: {}, since: {}, description: {}.",
            event.m_name, event.m_since, event.m_description);

        if (!event.m_args.empty()) {
            it =
                fmt::format_to(it, "        Args ({}):\n", event.m_args.size());
        }
        for (const auto &arg : event.m_args) {
            it = fmt::format_to(it, "    {}\n", arg);
        }

        return it;
    }
};
#endif
