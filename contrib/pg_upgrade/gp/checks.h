#ifndef GPDB_CHECKS_H
#define GPDB_CHECKS_H

/*
 * contrib/pg_upgrade/gp/checks.h
 *
 * Declarations of Greenplum-specific check functions
 */

#include "definitions.h"


bool check_external_partition(void);
bool check_covering_aoindex(void);
bool check_partition_indexes(void);
bool check_orphaned_toastrels(void);
bool check_online_expansion(void);
bool check_gphdfs_external_tables(void);
bool check_gphdfs_user_roles(void);


#endif //GPDB_CHECKS_H
