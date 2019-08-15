#ifndef GPDB_CHECK_UTILS_H
#define GPDB_CHECK_UTILS_H

#include "pg_upgrade.h"

/*
 * Greenplum-specific stuff
 */

/*
 * Check function definition.
 * Such functions actually perform checks on Greenplum
 */
typedef bool (*check_function)(void);

/*
 * A list of functions to be run to conduct tests
 */
extern check_function GP_CHECKS_LIST[];
extern size_t GP_CHECKS_LIST_LENGTH;

/*
 * gp_check_failure()
 *
 * Check if failure happened during check execution
 */
void
gp_check_failure(const char *restrict fmt,...)
__attribute__((format(PG_PRINTF_ATTRIBUTE, 1, 2)));

/*
 * Conducts check using provided check function
 */
void
conduct_check(bool (*check_function) (void))
__attribute__((nonnull(1)));


#endif //GPDB_CHECK_UTILS_H
