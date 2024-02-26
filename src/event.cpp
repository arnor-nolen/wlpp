#include <wlpp/event.hpp>

#include <tinyxml2.h>

#include <wlpp/util.hpp>

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

    // Args.
    const tinyxml2::XMLElement *argPtr = xmlElement->FirstChildElement("arg");

    while (argPtr != nullptr) {
        m_args.emplace_back(argPtr);
        argPtr = argPtr->NextSiblingElement("arg");
    }
}
