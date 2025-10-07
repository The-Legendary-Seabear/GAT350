#pragma once
#include "Math/Vector2.h"
#include "Math/Vector3.h"

#include <rapidjson/document.h>
#include <string>
#include <vector>

#define SERIAL_READ(value, data)			neu::serial::Read(value, #data, data)
#define SERIAL_READ_NAME(value, name, data)	neu::serial::Read(value, name, data)

#define SERIAL_CONTAINS(value, data)		value.HasMember(#data)
#define SERIAL_AT(value, data)				value[#data]

namespace neu::serial {
	using value_t = rapidjson::Value;
	using document_t = rapidjson::Document;

	bool Load(const std::string& filename, document_t& document);

	bool Read(const value_t& value, const std::string& name, bool& data, bool required = false);
	bool Read(const value_t& value, const std::string& name, int& data, bool required = false);
	bool Read(const value_t& value, const std::string& name, float& data, bool required = false);
	bool Read(const value_t& value, const std::string& name, std::string& data, bool required = false);
	bool Read(const value_t& value, const std::string& name, vec2& data, bool required = false);
	bool Read(const value_t& value, const std::string& name, vec3& data, bool required = false);
	bool Read(const value_t& value, const std::string& name, std::vector<int>& data, bool required = false);
}
