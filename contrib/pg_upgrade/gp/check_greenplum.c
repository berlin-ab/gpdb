/*
 * contrib/pg_upgrade/gp/check_greenplum.c
 *
 * Definition of an interface function to conduct Greenplum-specific pg_upgrade
 * checks
 */

#include "check_greenplum.h"
#include "definitions.h"


void
check_greenplum(void)
{
	size_t i;

	for (i = 0; i < GP_CHECKS_LIST_LENGTH; i++)
	{
		conduct_check(GP_CHECKS_LIST[i]);
	}
}
