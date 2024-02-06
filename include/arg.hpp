#ifndef ARG_HPP
#define ARG_HPP

#include <optional>
#include <string>

#include <fmt/core.h>
#include <tinyxml2.h>

class GeneratorHeader;
class GeneratorCode;

enum class ArgType {
    UnsignedInt,
    Int,
    NewId,
    FileDescriptor,
    String,
    Object,
};

auto strToArgType(std::string_view typeStr) -> ArgType;
auto argTypeToStr(ArgType argType) -> std::string;
auto argTypeToCppType(ArgType argType) -> std::string;

class Arg {
  public:
    explicit Arg(const tinyxml2::XMLElement *xmlElement) noexcept;

  private:
    std::string m_name{};
    ArgType m_type;
    std::optional<std::string> m_summary{};
    std::optional<std::string> m_interface{};
    bool m_allowNull{false};
    // unsigned int m_enum{};

    friend fmt::formatter<Arg>;
    friend GeneratorHeader;
    friend GeneratorCode;
};

template <>
class fmt::formatter<ArgType> {
  public:
    static constexpr auto parse(fmt::format_parse_context &ctx) {
        return ctx.begin();
    }

    template <typename Context>
    static constexpr auto format(const ArgType &argType, Context &ctx) {
        return fmt::format_to(ctx.out(), "{}", argTypeToStr(argType));
    }
};

template <>
class fmt::formatter<Arg> {
  public:
    static constexpr auto parse(fmt::format_parse_context &ctx) {
        return ctx.begin();
    }

    template <typename Context>
    static constexpr auto format(const Arg &arg, Context &ctx) {
        auto it = fmt::format_to(ctx.out(), "      Arg: {}, type: {}",
                                 arg.m_name, arg.m_type);

        if (arg.m_summary) {
            it = fmt::format_to(it, ", summary: {}", *arg.m_summary);
        }

        if (arg.m_interface) {
            it = fmt::format_to(it, ", interface: {}", *arg.m_interface);
        }

        it = fmt::format_to(it, ", allowNull: {}", arg.m_allowNull);

        it = fmt::format_to(it, ".");

        return it;
    }
};
#endif
