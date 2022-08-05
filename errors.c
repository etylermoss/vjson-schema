#define _GNU_SOURCE

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>
#include <sysexits.h>

#include "errors.h"

#define SILENT_SUCCESS true

err_s* get_err_s(err_t err);
void print_err_s(err_s* err);
void print_file_warning(const char* fpath, const char* err_str);

static err_s errors[] = {
	{ E_SUCCESS, EX_OK, "Success" },
	{ E_MISSING_ARG_SCHEMA, EX_USAGE, "Missing SCHEMA argument" },
	{ E_SCHEMA_ACCESS, EX_NOINPUT, "Could not access SCHEMA file" },
	{ E_SCHEMA_NOTFILE, EX_NOINPUT, "Schema is not a regular file" },
	{ E_VALIDATOR, EX_SOFTWARE, "Internal validator creation error" },
	{ E_TARGET_READ, EX_NOINPUT, "Could not open TARGET for reading" },
	{ E_TARGET_JSON, EX_DATAERR, "Could not parse TARGET as json" },
	{ E_STREAM, EX_IOERR, "Could not close file stream" },
	{ E_TARGET, EX_SOFTWARE, "Internal target handling error" },
};

err_s* get_err_s(err_t err)
{
	err_s* err_ptr = errors;
	err_s* err_ptr_end = errors + sizeof(errors)/sizeof(errors[0]);

	while (err_ptr < err_ptr_end && err_ptr->err != err)
		err_ptr++;

	return err_ptr != err_ptr_end ? err_ptr : NULL;
}

void print_err_s(err_s* err_s)
{
	if (!err_s)
	{
		fprintf(stderr, "%s: Error not found (internal code: %d)\n\n", program_invocation_short_name, err_s->err);
	}
	else if (err_s && err_s->err != E_SUCCESS)
	{
		fprintf(stderr, "%s: %s (internal code: %d)\n\n", program_invocation_short_name, err_s->msg, err_s->err);
	}
	else if (err_s && err_s->err == E_SUCCESS && !SILENT_SUCCESS)
	{
		fprintf(stdout, "%s: %s\n\n", program_invocation_short_name, err_s->msg);
	}
}

void print_file_warning(const char* fpath, const char* err_str)
{
	printf("Warning: %s (%s)\n", err_str, fpath);
}