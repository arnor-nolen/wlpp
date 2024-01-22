#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <vector>

#include <fmt/core.h>

#include <arg.hpp>

namespace tinyxml2 {
class XMLElement;
} // namespace tinyxml2

class GeneratorHeader;

enum class RequestType { Normal, Destructor };

auto strToRequestType(std::string_view typeStr) -> RequestType;
auto requestTypeToStr(RequestType requestType) -> std::string;

class Request {
  public:
    explicit Request(const tinyxml2::XMLElement *xmlElement) noexcept;

  private:
    std::string m_name{};
    unsigned int m_since{1};
    std::string m_description{};
    RequestType m_type{RequestType::Normal};
    std::vector<Arg> m_args{};

    friend fmt::formatter<Request>;
    friend GeneratorHeader;
};

template <>
class fmt::formatter<RequestType> {
  public:
    static constexpr auto parse(fmt::format_parse_context &ctx) {
        return ctx.begin();
    }

    template <typename Context>
    static constexpr auto format(const RequestType &requestType, Context &ctx) {
        return fmt::format_to(ctx.out(), "{}", requestTypeToStr(requestType));
    }
};

template <>
class fmt::formatter<Request> {
  public:
    static constexpr auto parse(fmt::format_parse_context &ctx) {
        return ctx.begin();
    }

    template <typename Context>
    static constexpr auto format(const Request &request, Context &ctx) {
        auto it = fmt::format_to(
            ctx.out(),
            "    Request: {}, since: {}, type: {}, description: {}.\n",
            request.m_name, request.m_since, request.m_type,
            request.m_description);

        if (!request.m_args.empty()) {
            it = fmt::format_to(it, "        Args ({}):\n",
                                request.m_args.size());
        }
        for (const auto &arg : request.m_args) {
            it = fmt::format_to(it, "    {}\n", arg);
        }

        return it;
    }
};
#endif
