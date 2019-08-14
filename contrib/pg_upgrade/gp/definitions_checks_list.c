/*
 * contrib/pg_upgrade/gp/check_gp_support.c
 *
 * A configuration file for actual check execution.
 *
 * Declarations of checks *normally* executed by Greenplum pg_upgrade
 */

#include "definitions.h"
#include "checks.h"


/* Check functions */

check_function GP_CHECKS_LIST[] = {
	check_online_expansion,
	check_external_partition,
	check_covering_aoindex,
	check_partition_indexes,
	check_orphaned_toastrels,
	check_gphdfs_external_tables,
	check_gphdfs_user_roles
};

size_t GP_CHECKS_LIST_LENGTH = sizeof(GP_CHECKS_LIST) / sizeof(GP_CHECKS_LIST[0]);
