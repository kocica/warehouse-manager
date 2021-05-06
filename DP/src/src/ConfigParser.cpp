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
    ConfigParser_t::ConfigParser_t()
        : exc{ false }
    {

    }

    ConfigParser_t::ConfigParser_t(const std::string& xmlFilename, bool exc)
        : exc{ exc }
    {
        tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();

        if(doc->LoadFile(xmlFilename.c_str()) != tinyxml2::XML_SUCCESS)
        {
            if(exc)
            {
                throw std::runtime_error("Failed to parse config XML!");
            }
            else
            {
                whm::Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_WARNING, "Failed to parse config XML!");
            }
        }

        tinyxml2::XMLElement* cfg = doc->FirstChildElement("configuration");

        if(!cfg)
        {
            if(exc)
            {
                throw std::runtime_error("Failed to parse config XML!");
            }
            else
            {
                whm::Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_WARNING, "Failed to parse config XML!");
            }
        }

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
            if(exc)
            {
                throw std::runtime_error(std::string("Field ") + name + std::string(" not found!"));
            }

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

    void ConfigParser_t::set(const std::string& key, const std::string& value)
    {
        //parsedValues.insert(std::make_pair(key, value));
        parsedValues[key] = value;
    }

    void ConfigParser_t::dump() const
    {
        whm::Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_DEBUG, "Configuration dump");

        for(const auto& val : parsedValues)
        {
            whm::Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_DEBUG, "Name <%s> = Value <%s>", val.first.c_str(), val.second.c_str());
        }
    }

    void ConfigParser_t::exportToFile(const std::string& xmlFilename)
    {
        tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();
        tinyxml2::XMLElement* config = doc->NewElement( "configuration" );

        std::for_each(parsedValues.begin(), parsedValues.end(),
                      [&config](const auto& parsedValue) -> void
                      {
                          config->SetAttribute( parsedValue.first.c_str(),
                                                parsedValue.second.c_str() );
                      });

        doc->InsertEndChild(config);
        doc->SaveFile(xmlFilename.c_str());
        delete doc;
    }
}