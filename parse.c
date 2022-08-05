#define _GNU_SOURCE
#define _XOPEN_SOURCE 700

#include <errno.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <ftw.h>
#include <string.h>

#include "errors.h"
#include "parse.h"
#include "validate.h"

#ifndef USE_FDS
#define USE_FDS 15
#endif

extern err_t cpp_create_validator(FILE* fschema, validator_t* validator);
extern void cpp_delete_validator(const validator_t validator);
extern err_t cpp_validate(const char* fpath, const validator_t validator, bool* result);

err_t create_validator(const char* fpath, validator_t* validator, ino_t* schema_inode);
void delete_validator(const validator_t schema);
err_t walk_target(const char* fpath, const validator_t schema, const ino_t schema_inode, const int depth, int* compared_valid, int* compared_total);
static int walk_target_file(const char* fpath, const struct stat* sb, int tflag, struct FTW* ftwbuf);
static err_t validate_file(const char* fpath);

static struct {
	validator_t validator;
	ino_t schema_inode;
	int depth;
	int* compared_valid;
	int* compared_total;
} target_args = {
	NULL,
	0UL,
	0,
	NULL,
	NULL,
};

err_t create_validator(const char* fpath, validator_t* validator, ino_t* schema_inode)
{
	validator_t new_validator = NULL;
	*validator = NULL;
	*schema_inode = 0;

	int fdschema;
	if ((fdschema = open(fpath, O_RDONLY)) == -1)
		return E_SCHEMA_ACCESS;

	struct stat buf;
	fstat(fdschema, &buf);
	if ((buf.st_mode & S_IFMT) != S_IFREG)
		return E_SCHEMA_NOTFILE;

	FILE* fschema;
	if ((fschema = fdopen(fdschema, "r")) == NULL)
		return E_SCHEMA_ACCESS;

	err_t err;
	if ((err = cpp_create_validator(fschema, &new_validator)) == E_SUCCESS)
	{
		*schema_inode = buf.st_ino;
		*validator = new_validator;
	}

	fclose(fschema);
	return err;
}

void delete_validator(const validator_t validator)
{
	cpp_delete_validator(validator);
}

err_t walk_target(const char* fpath, const validator_t validator, const ino_t schema_inode, const int depth, int* compared_valid, int* compared_total)
{
	target_args.validator = validator;
	target_args.schema_inode = schema_inode;
	target_args.depth = depth;
	target_args.compared_valid = compared_valid;
	target_args.compared_total = compared_total;

	int err;
	if ((err = nftw(fpath, walk_target_file, USE_FDS, FTW_PHYS)) == 0)
	{
		return E_SUCCESS;
	}

	if (err == -1)
	{
		print_file_warning(fpath, strerror(errno));
	}

	return E_SUCCESS;
}

static int walk_target_file(const char* fpath, const struct stat* sb, int tflag, struct FTW* ftwbuf)
{
	if (sb->st_ino == target_args.schema_inode)
	{
		return 0;
	}

	if (target_args.depth >= 0 && ftwbuf->level > target_args.depth)
	{
		return 0; 
	}
	
	if (tflag == FTW_F)
	{
		const char* base_name = fpath + ftwbuf->base;
		const int base_name_l = strlen(base_name);

		if (base_name_l > 5 && strcmp(base_name + base_name_l - 5, ".json") == 0)
		{
			bool result;
			err_t err = cpp_validate(fpath, target_args.validator, &result);
			if (result && err == E_SUCCESS)
			{
				(*target_args.compared_valid)++;
				(*target_args.compared_total)++;
			}
			else if (err == E_SUCCESS)
			{
				(*target_args.compared_total)++;
			}

			return (int) err;
		}
	}

	return 0;
}