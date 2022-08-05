#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif

typedef void* validator_t;

err_t cpp_create_validator(FILE* fschema, validator_t* validator);
void cpp_delete_validator(const validator_t validator); // make const?
err_t cpp_validate(const char* fpath, const validator_t validator, bool* result);

#ifdef __cplusplus
}
#endif