#include "Json.h"
#include "File.h"
#include "Logger.h"

#include <rapidjson/istreamwrapper.h>
#include <iostream>

namespace neu::serial
{
    bool Load(const std::string& filename, document_t& document) {
        // read the file into a string
        std::string buffer;
        if (!file::ReadTextFile(filename, buffer)) {
            LOG_ERROR("Could not read file: {}.", filename);
            return false;
        }

        // convert the string into a json stream
        std::stringstream stream(buffer);
        rapidjson::IStreamWrapper istream(stream);

        // set the json document from the stream
        document.ParseStream(istream);
        // check if the parse was successful
        if (!document.IsObject()) {
            LOG_ERROR("Could not parse Json: {}.", filename);
            return false;
        }

        return true;
    }

    bool Read(const value_t& value, const std::string& name, int& data, bool required) {
        // check if the value has the "<name>" and the correct data type
        if (!value.HasMember(name.c_str()) || !value[name.c_str()].IsInt()) {
            if (required) LOG_ERROR("Could not read Json value (int): {}.", name);
            return false;
        }

        // get the data
        data = value[name.c_str()].GetInt();

        return true;
    }

    bool Read(const value_t& value, const std::string& name, float& data, bool required)
    {
        // check if the value has the "<name>" and the correct data type
        if (!value.HasMember(name.c_str()) || !value[name.c_str()].IsNumber()) {
            if (required) LOG_ERROR("Could not read Json value (float): {}.", name);
            return false;
        }

        // get the data
        data = value[name.c_str()].GetFloat();

        return true;
    }

    bool Read(const value_t& value, const std::string& name, std::string& data, bool required)
    {
        if (!value.HasMember(name.c_str()) || !value[name.c_str()].IsString()) {
            if (required) LOG_ERROR("Could not read Json value (string): {}.", name);
            return false;
        }

        data = value[name.c_str()].GetString();

        return true;
    }

    bool Read(const value_t& value, const std::string& name, bool& data, bool required) {
        // check if the value has the "<name>" and the correct data type
        if (!value.HasMember(name.c_str()) || !value[name.c_str()].IsBool())
        {
            if (required) LOG_ERROR("Could not read Json value (bool): {}.", name);
            return false;
        }

        // get the data
        data = value[name.c_str()].GetBool();

        return true;
    }

    bool Read(const value_t& value, const std::string& name, glm::vec2& data, bool required) {
        // check if the value has the "<name>" and is an array with 2 elements
        if (!value.HasMember(name.c_str()) || !value[name.c_str()].IsArray() || value[name.c_str()].Size() != 2) {
            if (required) LOG_ERROR("Could not read Json value (vec2): {}.", name);
            return false;
        }

        // get json array object
        auto& array = value[name.c_str()];
        // get array values
        for (rapidjson::SizeType i = 0; i < array.Size(); i++) {
            if (!array[i].IsNumber()) {
                LOG_ERROR("Could not read Json value: {}.", name);
                return false;
            }

            // get the data
            data[i] = array[i].GetFloat();
        }

        return true;
    }

    bool Read(const value_t& value, const std::string& name, glm::vec3& data, bool required)
    {
        // check if the value has the "<name>" and is an array with 3 elements
        if (!value.HasMember(name.c_str()) || !value[name.c_str()].IsArray() || value[name.c_str()].Size() != 3) {
            if (required) LOG_ERROR("Could not read Json value (vec2): {}.", name);
            return false;
        }

        // get json array object
        auto& array = value[name.c_str()];
        // get array values
        for (rapidjson::SizeType i = 0; i < array.Size(); i++) {
            if (!array[i].IsNumber()) {
                LOG_ERROR("Could not read Json value: {}.", name);
                return false;
            }

            // get the data
            data[i] = array[i].GetFloat();
        }

        return true;
    }


    bool Read(const value_t& value, const std::string& name, std::vector<int>& data, bool required)
    {
        // check if the value has the "<name>" and is an array with 3 elements
        if (!value.HasMember(name.c_str()) || !value[name.c_str()].IsArray()) {
            if (required) LOG_ERROR("Could not read Json value (vector<int>): {}.", name);
            return false;
        }

        // get json array object
        auto& array = value[name.c_str()];
        // get array values
        for (rapidjson::SizeType i = 0; i < array.Size(); i++) {
            if (!array[i].IsNumber()) {
                LOG_ERROR("Could not read Json value: {}.", name);
                return false;
            }

            // get the data
            data.push_back(array[i].GetInt());
        }

        return true;
    }
}
