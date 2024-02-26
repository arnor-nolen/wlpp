#include <wlpp/request.hpp>

#include <string_view>

#include <tinyxml2.h>

#include <wlpp/util.hpp>

auto strToRequestType(std::string_view typeStr) -> RequestType {
    if (typeStr == "destructor") {
        return RequestType::Destructor;
    }

    halt("Unknown request type.");
};

auto requestTypeToStr(RequestType requestType) -> std::string {
    switch (requestType) {
    case RequestType::Normal:
        return "normal";
    case RequestType::Destructor:
        return "destructor";
    default:
        halt("Unknown RequestType.");
    }
}

Request::Request(const tinyxml2::XMLElement *xmlElement) noexcept {
    const char *requestName = xmlElement->Attribute("name");
    nullptrCheck(requestName);
    m_name = requestName;

    xmlElement->QueryUnsignedAttribute("since", &m_since);

    const char *requestTypeStr = xmlElement->Attribute("type");
    if (requestTypeStr != nullptr) {
        m_type = strToRequestType(requestTypeStr);
    }

    // Description.
    const auto *descPtr = xmlElement->FirstChildElement("description");
    nullptrCheck(descPtr);

    const char *descText = descPtr->Attribute("summary");
    nullptrCheck(descText);
    m_description = descText;

    // Args.
    const tinyxml2::XMLElement *argPtr = xmlElement->FirstChildElement("arg");

    while (argPtr != nullptr) {
        m_args.emplace_back(argPtr);
        argPtr = argPtr->NextSiblingElement("arg");
    }
}
