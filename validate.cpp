#include <iostream>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>

#include "errors.h"
#include "validate.h"

using nlohmann::json;
using nlohmann::json_schema::json_validator;

extern "C" err_t cpp_create_validator(FILE* fschema, validator_t* validator);
extern "C" void cpp_delete_validator(const validator_t validator);
extern "C" err_t cpp_validate(const char* fpath, const validator_t validator, bool* result);
static void format_checker(const std::string &format, const std::string &value);

extern "C" err_t cpp_create_validator(FILE* fschema, validator_t* validator)
{
	try
	{
		json schema = json::parse(fschema, nullptr, true, true);
		json_validator* new_validator = new json_validator(nullptr, format_checker);
		new_validator->set_root_schema(schema);
		*validator = new_validator;
		return E_SUCCESS;
	}
	catch (const std::exception &e)
	{
		std::cerr << "Could not parse schema " << e.what() << "\n";
		*validator = NULL;
		return E_VALIDATOR;
	}
}

extern "C" void cpp_delete_validator(const validator_t validator_ptr)
{
	json_validator* validator = static_cast<json_validator*>(validator_ptr);
	delete validator;
}

extern "C" err_t cpp_validate(const char* fpath, const validator_t validator_ptr, bool* result)
{
	json_validator* validator = static_cast<json_validator*>(validator_ptr);

	FILE* target_file;
	if ((target_file = fopen(fpath, "r")) == NULL)
	{
		print_file_warning(fpath, "Could not open for reading");
		return E_TARGET_READ;
	}

	json target;
	if ((target = json::parse(target_file, nullptr, false, true)).is_discarded())
	{
		print_file_warning(fpath, "Could not parse as json");
		return E_TARGET_JSON;
	}

	if (fclose(target_file) != 0)
	{
		print_file_warning(fpath, "Could not close file stream");
		return E_STREAM;
	}

	bool valid = true;
	try
	{
		validator->validate(target);
	}
	catch (const std::exception& e)
	{
		valid = false;
		printf("%s: Invalid\n", fpath);
		printf("%s: %s\n", fpath, e.what());
	}

	if (valid)
	{
		printf("%s: Valid\n", fpath);
	}

	if (result != NULL)
	{
		*result = valid;
	}

	return E_SUCCESS;
}

// TODO: check the month / day combo can actually exist, sounds like a lot of effort ...

static void format_checker(const std::string& format, const std::string& str)
{
	if (format == "date") {
		if (str.length() != 10)
			throw std::invalid_argument("Invalid date " + str);

		if (str[4] != '-' || str[7] != '-')
			throw std::invalid_argument("Invalid date " + str);

		int year, month, day;
		std::size_t len;

		year = std::stoi(str.c_str() + 0, &len, 10);

		if (year < 0 || len != 4)
			throw std::invalid_argument("Invalid year " + str);

		month = std::stoi(str.c_str() + 5, &len, 10);

		if (month < 1 || month > 12 || len != 2)
			throw std::invalid_argument("Invalid month " + str);

		day = std::stoi(str.c_str() + 8, &len, 10);

		if (day < 1 || day > 31 || len != 2)
			throw std::invalid_argument("Invalid day " + str);
	} else {
		throw std::logic_error("Don't know how to validate " + format);
	}
}