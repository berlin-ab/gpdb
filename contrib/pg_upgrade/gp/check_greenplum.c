/*
 * contrib/pg_upgrade/gp/check_greenplum.c
 *
 * Definition of an interface function to conduct Greenplum-specific pg_upgrade
 * checks
 */

#include "pg_upgrade.h"


#include "check_greenplum.h"


static void
conduct_check(bool (*check_function) (void))
{
	if (check_function())
	{
		check_ok();
		return;
	}

	pg_log(PG_FATAL, "One or more checks failed. See output above.\n");
}


void
check_greenplum(check_function gp_checks_list[], int gp_checks_list_length)
{
	size_t i;

	for (i = 0; i < gp_checks_list_length; i++)
	{
		conduct_check(gp_checks_list[i]);
	}
}
