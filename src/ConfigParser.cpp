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
#include "Logger.h"
#include "ConfigParser.h"

namespace whm
{
    ConfigParser_t::ConfigParser_t(const std::string& xmlFilename)
    {
        tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();

        doc->LoadFile(xmlFilename.c_str());

        tinyxml2::XMLElement* cfg = doc->FirstChildElement("optimizer_config");

        const tinyxml2::XMLAttribute* attribute = cfg->FirstAttribute();

        while(attribute)
        {
            parsedValues.insert(std::make_pair(attribute->Name(), attribute->Value()));

            attribute = attribute->Next();
        }

        dump();

        delete doc;
    }

    std::string ConfigParser_t::findByName(const std::string& name)
    {
        auto it = parsedValues.find(name);

        if(it == parsedValues.end())
        {
            whm::Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_WARNING, "Field <%s> not found!", name.c_str());
            return std::string();
        }

        return it->second;
    }

    template<>
    int32_t ConfigParser_t::getAs(const std::string& name)
    {
        return std::stoi(findByName(name));
    }

    template<>
    double ConfigParser_t::getAs(const std::string& name)
    {
        return std::stod(findByName(name));
    }

    template<>
    std::string ConfigParser_t::getAs(const std::string& name)
    {
        return findByName(name);
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