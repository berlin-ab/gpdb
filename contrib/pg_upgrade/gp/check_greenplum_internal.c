#include "pg_upgrade.h"

#include "check_greenplum_internal.h"


static void
conduct_check(bool (*check_function) (void))
{
	if (check_function())
	{
		check_ok();
		return;
	}

	check_failed();
}


void
perform_greenplum_checks(check_function gp_checks_list[], int gp_checks_list_length)
{
	size_t i;

	for (i = 0; i < gp_checks_list_length; i++)
	{
		conduct_check(gp_checks_list[i]);
	}
}
