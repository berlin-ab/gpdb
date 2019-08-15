#ifndef GPDB_CHECKS_H
#define GPDB_CHECKS_H

/*
 * contrib/pg_upgrade/gp/checks.h
 *
 * Declarations of Greenplum-specific check functions
 */

#include "definitions.h"


extern bool check_external_partition(void);
extern bool check_covering_aoindex(void);
extern bool check_partition_indexes(void);
extern bool check_orphaned_toastrels(void);
extern bool check_online_expansion(void);
extern bool check_gphdfs_external_tables(void);
extern bool check_gphdfs_user_roles(void);


#endif //GPDB_CHECKS_H
