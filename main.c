#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <argz.h>
#include <unistd.h>
#include <sysexits.h>
#include <string.h>
#include <stdbool.h>

#include "errors.h"
#include "parse.h"

int main(int argc, char* argv[]);
int parse_opt(int key, char *arg, struct argp_state *state);

static struct arguments {
	int depth;
	char* argz;
	size_t argz_len;
} arguments = {
	.depth = -1,
	.argz = NULL,
	.argz_len = 0,
};

const char *argp_program_version = "version 1.0.0";
const char *argp_program_bug_address = "<eden@etylermoss.com>";

int main(int argc, char* argv[])
{
	err_t err = E_SUCCESS;
	validator_t validator = NULL;
	ino_t schema_inode = 0;
	int compared_total = 0, compared_valid = 0;

	const struct argp_option options[] =
	{
		{ "SCHEMA", 0, 0, OPTION_DOC | OPTION_NO_USAGE, "Path to JSON schema", 0 },
		{ "TARGET", 0, 0, OPTION_DOC | OPTION_NO_USAGE, "Paths to validate against schema", 0 },
		{ 0, 0, 0, OPTION_DOC | OPTION_NO_USAGE, "OPTIONS:", 1 },
		{ "depth", 'd', "INT", 0, "Maximum depth to search", 1 },
		{ 0 },
	};
	const struct argp argp = { options, parse_opt, "SCHEMA [TARGET...]", "C/C++ tool to verify that a JSON file (or files) matches a given JSON Schema. Uses nlohmann/json-schema for validation." };
	if ((err = (err_t) argp_parse(&argp, argc, argv, 0, 0, &arguments)) != E_SUCCESS)
		goto cleanup;

	if ((err = create_validator(arguments.argz, &validator, &schema_inode)) != E_SUCCESS)
		goto cleanup;

	if (argz_count(arguments.argz, arguments.argz_len) < 2)
	{	
		/* no target supplied, use cwd */
		char* cwd = get_current_dir_name();
		err = walk_target(cwd, validator, schema_inode, arguments.depth, &compared_valid, &compared_total);
		free(cwd);

		if (err != E_SUCCESS)
			goto cleanup;
	}
	else
	{	
		/* handle remaining args as targets */
		const char* prev,* arg;
		prev = arg = argz_next(arguments.argz, arguments.argz_len, NULL);

		while ((arg = argz_next(arguments.argz, arguments.argz_len, prev)) != NULL)
		{
			if ((err = walk_target(arg, validator, schema_inode, arguments.depth, &compared_valid, &compared_total)) != E_SUCCESS)
				goto cleanup;
			
			prev = arg;
		}
	}
cleanup:
	err_s* err_struct = get_err_s(err);
	print_err_s(err_struct);
	printf("\n%d/%d files were valid.\n", compared_valid, compared_total);

	if (arguments.argz_len > 0)
		free(arguments.argz);
	if (validator)
		delete_validator(validator);

	return err_struct->exit;
}

int parse_opt(int key, char* arg, struct argp_state* state)
{
	struct arguments* arguments = (struct arguments*) state->input;
	int depth;
	size_t argz_c;

	switch (key)
	{
		case 'd':
			if ((depth = atoi(arg)) >= 0)
				arguments->depth = depth;
			break;
		case ARGP_KEY_ARG:
			argz_add(&arguments->argz, &arguments->argz_len, arg);
			break;
		case ARGP_KEY_INIT:
			arguments->argz = NULL;
			arguments->argz_len = 0;
			break;
		case ARGP_KEY_END:
			argz_c = argz_count(arguments->argz, arguments->argz_len);
			if (argz_c < 1)
				return E_MISSING_ARG_SCHEMA;
			break;
		default:
			return ARGP_ERR_UNKNOWN;
	}
	
	return E_SUCCESS;
}