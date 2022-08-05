#include "validate.h"

err_t create_validator(const char* fpath, validator_t* schema, ino_t* schema_inode);
void delete_validator(const validator_t schema);
err_t walk_target(const char* fpath, const validator_t schema, const ino_t schema_inode, const int depth, int* compared_valid, int* compared_total);