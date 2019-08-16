#ifndef GPDB_CHECK_GREENPLUM_INTERNAL_H
#define GPDB_CHECK_GREENPLUM_INTERNAL_H

#include "c.h"

typedef bool (*check_function)(void);

extern void perform_greenplum_checks(check_function gp_checks_list[], int gp_checks_list_length);

#endif // GPDB_CHEC_GREENPLUM_INTERNAL_H