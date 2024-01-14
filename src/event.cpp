#include <event.hpp>

#include <tinyxml2.h>

#include <util.hpp>

Event::Event(const tinyxml2::XMLElement *xmlElement) noexcept {
    const char *eventName = xmlElement->Attribute("name");
    nullptrCheck(eventName);
    m_name = eventName;

    xmlElement->QueryUnsignedAttribute("since", &m_since);

    // Description.
    const auto *descPtr = xmlElement->FirstChildElement("description");
    nullptrCheck(descPtr);

    const char *descText = descPtr->Attribute("summary");
    nullptrCheck(descText);
    m_description = descText;
}
