#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>

#include <fmt/core.h>

namespace tinyxml2 {
class XMLElement;
} // namespace tinyxml2

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
    // args

    friend fmt::formatter<Request>;
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
        return fmt::format_to(
            ctx.out(), "    Request: {}, since: {}, type: {}, description: {}.",
            request.m_name, request.m_since, request.m_type,
            request.m_description);
    }
};
#endif
