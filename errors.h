#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

typedef enum {
	E_SUCCESS = 0,
	E_MISSING_ARG_SCHEMA,
	E_SCHEMA_ACCESS,
	E_SCHEMA_NOTFILE,
	E_VALIDATOR,
	E_TARGET_READ,
	E_TARGET_JSON,
	E_STREAM,
	E_TARGET,
} err_t;

typedef const struct {
	err_t err;		/* internal error code */
	uint8_t exit;	/* exit status code */
	char* msg;		/* error message */
} err_s;

err_s* get_err_s(err_t err);
void print_err_s(err_s* err_s);
void print_file_warning(const char* fpath, const char* err_str);

#ifdef __cplusplus
}
#endif