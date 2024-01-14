#include <interface.hpp>

#include <util.hpp>

#include <tinyxml2.h>

Interface::Interface(const tinyxml2::XMLElement *xmlElement) noexcept {
    const char *interfaceName = xmlElement->Attribute("name");
    nullptrCheck(interfaceName);
    m_name = interfaceName;

    xmlElement->QueryUnsignedAttribute("version", &m_version);

    // Description.
    const auto *descPtr = xmlElement->FirstChildElement("description");
    nullptrCheck(descPtr);

    const char *descText = descPtr->Attribute("summary");
    nullptrCheck(descText);
    m_description = descText;

    // Requests.
    const tinyxml2::XMLElement *requestPtr =
        xmlElement->FirstChildElement("request");

    while (requestPtr != nullptr) {
        m_requests.emplace_back(requestPtr);
        requestPtr = requestPtr->NextSiblingElement("request");
    }

    // Events.
    const tinyxml2::XMLElement *eventPtr =
        xmlElement->FirstChildElement("event");

    while (eventPtr != nullptr) {
        m_events.emplace_back(eventPtr);
        eventPtr = eventPtr->NextSiblingElement("event");
    }
}
