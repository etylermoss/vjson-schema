#include <argp.h>
#include <argz.h>
#include <sysexits.h>
#include <unistd.h>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <filesystem>

#include "validator.hpp"

int main(int argc, char* argv[]);
int parse_opt(int key, char *arg, struct argp_state *state);

static struct
{
	bool verbose {};
	std::optional<int> recurse;
	std::filesystem::path schema;
	std::vector<std::filesystem::path> targets;
} arguments;

[[maybe_unused]] const char* argp_program_version = "vjson-schema 1.1.0";
[[maybe_unused]] const char* argp_program_bug_address = "<eden@etylermoss.com>";

int main(int argc, char* argv[])
{
	try
	{
		const argp_option options[] =
		{
			{ "SCHEMA", 0, nullptr, OPTION_DOC | OPTION_NO_USAGE, "Path to JSON schema", 0 },
			{ "TARGET", 0, nullptr, OPTION_DOC | OPTION_NO_USAGE, "Paths to validate against schema", 0 },
			{ nullptr, 0, nullptr, OPTION_DOC | OPTION_NO_USAGE, "OPTIONS:", 1 },
			{ "recurse", 'r', "DEPTH", OPTION_ARG_OPTIONAL, "Recurse to depth", 1 },
			{ "verbose", 'v', nullptr, 0, "Verbose output", 1 },
			{ nullptr },
		};

		const argp argp = { options, parse_opt, "SCHEMA [TARGET...]", "Validate JSON file(s) against a JSON schema." };

		argp_parse(&argp, argc, argv, 0, nullptr, nullptr);

		validator validator (arguments.schema, arguments.recurse, arguments.verbose);

		if (!isatty(fileno(stdin)))
		{
			/* read json from stdin */
			validator.validate(std::cin);

			if (!validator.get_valid())
				return EX_DATAERR;
		}
		else
		{
			if (arguments.targets.empty())
			{
				/* no target supplied, use cwd */
				validator.validate(std::filesystem::current_path());
			}
			else
			{
				/* remaining args handled as targets */
				for (const auto& target : arguments.targets)
				{
					validator.validate(target);
				}
			}

			std::cout << validator.get_valid() << " of " << validator.get_total() << " files(s) were valid\n";
		}
	}
	catch (const std::invalid_argument& ex)
	{
		std::cerr << ex.what() << '\n';

		return EX_USAGE;
	}
	catch (const std::exception& ex)
	{
		std::cerr << ex.what() << '\n';

		return EX_SOFTWARE;
	}

	return EX_OK;
}

int parse_opt(int key, char* arg, argp_state* state)
{
	switch (key)
	{
		case 'r':
			if (arg == nullptr)
			{
				arguments.recurse = 0;
			}
			else
			{
				try
				{
					size_t pos;
					arguments.recurse = std::stoi(arg, &pos);
					if (arguments.recurse < 0)
						throw std::out_of_range("");
					if (pos != strlen(arg))
						throw std::invalid_argument("");
				}
				catch (std::out_of_range const& ex)
				{
					throw std::invalid_argument("Recurse depth out of range (" + std::string(arg) + ")");

				}
				catch (std::invalid_argument const& ex)
				{
					throw std::invalid_argument("Recurse depth invalid (" + std::string(arg) + ")");
				}
			}
			break;
		case 'v':
			arguments.verbose = true;
			break;
		case ARGP_KEY_ARG:
			if (state->arg_num == 0)
				arguments.schema = arg;
			else
				arguments.targets.emplace_back(arg);
			break;
		case ARGP_KEY_END:
			if (arguments.schema.empty())
				throw std::invalid_argument("Missing SCHEMA argument");
			break;
		default:
			return ARGP_ERR_UNKNOWN;
	}

	return 0;
}