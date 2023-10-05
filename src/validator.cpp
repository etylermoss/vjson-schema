#include <filesystem>
#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>

#include "validator.hpp"

validator::validator(const std::filesystem::path& schema, const std::optional<int>& recurse, bool verbose)
:
	schema {schema},
	recurse {recurse},
	verbose {verbose},
	compared_total {0},
	compared_valid {0}
{
	try
	{
		std::ifstream schema_stream (schema);

		nlohmann::json schema_json = nlohmann::json::parse(schema_stream, nullptr, true, true);

		json_validator = {nullptr, nlohmann::json_schema::default_string_format_check};

		json_validator.set_root_schema(schema_json);
	}
	catch (const std::exception& ex)
	{
		throw std::invalid_argument("Could not read SCHEMA file");
	}
}

void validator::validate(std::istream& target)
{
	validate_stream(target);
}

void validator::validate(const std::filesystem::path& target)
{
	if (std::filesystem::is_regular_file(target))
		validate_file(target);
	else
		validate_directory(target);
}

int validator::get_total() const
{
	return compared_total;
}

int validator::get_valid() const
{
	return compared_valid;
}

void validator::validate_stream(std::istream& target)
{
	try
	{
		compared_total++;

		nlohmann::json target_json = nlohmann::json::parse(target, nullptr, true, true);

		json_validator.validate(target_json);

		compared_valid++;

		std::cout << "[valid]\n";
	}
	catch (const std::exception& ex)
	{
		std::cout << "[invalid]\n";

		if (verbose)
			std::cout << ex.what();
	}
}

void validator::validate_file(const std::filesystem::path& target)
{
	try
	{
		compared_total++;

		std::ifstream target_stream (target);

		nlohmann::json target_json = nlohmann::json::parse(target_stream, nullptr, true, true);

		json_validator.validate(target_json);

		compared_valid++;

		std::cout << "[valid]   : " << target.string() << '\n';
	}
	catch (const std::exception& ex)
	{
		std::cout << "[invalid] : " << target.string() << '\n';
		if (verbose)
			std::cout << ex.what();
	}
}

void validator::validate_directory(const std::filesystem::path& target)
{
	std::filesystem::directory_options iterator_options {std::filesystem::directory_options::skip_permission_denied};

	for
	(
		auto i = std::filesystem::recursive_directory_iterator {target, iterator_options};
		i != std::filesystem::recursive_directory_iterator();
		++i
	)
	{
		if (i.depth() > recurse.value_or(0))
			continue;

		if (i->is_directory())
			continue;

		if (i->path() == schema)
			continue;

		std::string extension {i->path().extension()};

		std::transform(extension.begin(), extension.end(), extension.begin(), tolower);

		if (extension != ".json")
			continue;

		validate_file(i->path());
	}
}
