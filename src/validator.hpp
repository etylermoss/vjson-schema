#ifndef VJSON_SCHEMA_VALIDATOR_HPP
#define VJSON_SCHEMA_VALIDATOR_HPP

#include <string>
#include <optional>

#include <nlohmann/json-schema.hpp>

class validator
{
public:
	validator(const std::filesystem::path& schema, const std::optional<int>& recurse, bool verbose);

	void validate(std::istream& target);
	void validate(const std::filesystem::path& target);

	[[nodiscard]] int get_total() const;
	[[nodiscard]] int get_valid() const;

private:
	const std::filesystem::path& schema;
	const std::optional<int>& recurse;
	bool verbose;
	int compared_total;
	int compared_valid;
	nlohmann::json_schema::json_validator json_validator;

	void validate_stream(std::istream& target);
	void validate_file(const std::filesystem::path& target);
	void validate_directory(const std::filesystem::path& target);
};


#endif //VJSON_SCHEMA_VALIDATOR_HPP
