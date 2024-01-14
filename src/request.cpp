#include <request.hpp>

#include <tinyxml2.h>

#include <util.hpp>

Request::Request(const tinyxml2::XMLElement *xmlElement) noexcept {
    const char *requestName = xmlElement->Attribute("name");
    nullptrCheck(requestName);
    m_name = requestName;

    xmlElement->QueryUnsignedAttribute("since", &m_since);

    // Description.
    const auto *descPtr = xmlElement->FirstChildElement("description");
    nullptrCheck(descPtr);

    const char *descText = descPtr->Attribute("summary");
    nullptrCheck(descText);
    m_description = descText;
}
