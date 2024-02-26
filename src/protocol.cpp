#include <wlpp/protocol.hpp>

#include <tinyxml2.h>

#include <wlpp/util.hpp>

Protocol::Protocol(const tinyxml2::XMLElement *xmlElement) noexcept {
    const char *protocolName = xmlElement->Attribute("name");
    nullptrCheck(protocolName);
    m_name = protocolName;

    const auto *copyrightPtr = xmlElement->FirstChildElement("copyright");
    nullptrCheck(copyrightPtr);

    const char *copyrightText = copyrightPtr->GetText();
    nullptrCheck(copyrightText);
    m_copyright = copyrightText;

    // Interfaces.
    const auto *interfacePtr = xmlElement->FirstChildElement("interface");

    while (interfacePtr != nullptr) {
        m_interfaces.emplace_back(interfacePtr);
        interfacePtr = interfacePtr->NextSiblingElement("interface");
    }
}
