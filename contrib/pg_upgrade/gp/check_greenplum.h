#ifndef GPDB_CHECK_GREENPLUM_H
#define GPDB_CHECK_GREENPLUM_H

/*
 * contrib/pg_upgrade/gp/check_greenplum.h
 *
 * Declaration of an interface function to conduct Greenplum-specific pg_upgrade
 * checks
 */


#include <stdlib.h>
#include "c.h"

/*
 * Conduct all greenplum checks, defined in GP_CHECKS_LIST
 *
 * This function should be executed after all PostgreSQL checks. The order of the checks should not matter.
 */
typedef bool (*check_function)(void);


extern void
gp_check_failure(const char *restrict fmt,...)
__attribute__((format(printf, 1, 2)));

/*
 * Conducts check using provided check function
 */
extern void
conduct_check(bool (*check_function) (void))
__attribute__((nonnull(1)));


extern void check_greenplum(check_function gp_checks_list[], int gp_checks_list_length);


#endif //GPDB_CHECK_GREENPLUM_H
