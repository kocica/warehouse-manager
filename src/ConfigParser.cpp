/**
 * Warehouse manager
 *
 * @file    ConfigParser.cpp
 * @date    10/24/2020
 * @author  Filip Kocica
 * @brief   Simple configuration parser module
 */

// Std
#include <utility>
#include <type_traits>

// TinyXML
#include "tinyxml2.h"

// Local
#include "Utils.h"
#include "Logger.h"
#include "ConfigParser.h"

namespace whm
{
    ConfigParser_t::ConfigParser_t(const std::string& xmlFilename)
    {
        tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();

        doc->LoadFile(xmlFilename.c_str());

        tinyxml2::XMLElement* cfg = doc->FirstChildElement("configuration");

        const tinyxml2::XMLAttribute* attribute = cfg->FirstAttribute();

        while(attribute)
        {
            parsedValues.insert(std::make_pair(attribute->Name(), attribute->Value()));

            attribute = attribute->Next();
        }

        dump();

        delete doc;
    }

    std::string ConfigParser_t::findByName(const std::string& name) const
    {
        auto it = parsedValues.find(name);

        if(it == parsedValues.end())
        {
            whm::Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_WARNING, "Field <%s> not found!", name.c_str());
            return std::string();
        }

        return it->second;
    }

    bool ConfigParser_t::isSet(const std::string& name) const
    {
        return parsedValues.find(name) != parsedValues.end() && findByName(name) != std::string();
    }

    template<>
    int32_t ConfigParser_t::getAs(const std::string& name) const
    {
        return std::stoi(findByName(name));
    }

    template<>
    double ConfigParser_t::getAs(const std::string& name) const
    {
        return std::stod(findByName(name));
    }

    template<>
    std::string ConfigParser_t::getAs(const std::string& name) const
    {
        return findByName(name);
    }

    template<>
    bool ConfigParser_t::getAs(const std::string& name) const
    {
        return utils::toLower(findByName(name)) == "true";
    }

    void ConfigParser_t::dump() const
    {
        whm::Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_DEBUG, "Configuration dump");

        for(const auto& val : parsedValues)
        {
            whm::Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_DEBUG, "Name <%s> = Value <%s>", val.first.c_str(), val.second.c_str());
        }
    }
}