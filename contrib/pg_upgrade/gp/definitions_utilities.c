#include "definitions.h"

/*
 * contrib/pg_upgrade/check_gp/check_gp_support.c
 *
 * Definitions of utility functions *normally* used to conduct pg_upgrade checks
 */

void
gp_check_failure(const char *restrict fmt,...)
{
	report_status(PG_REPORT, "fatal\n");

	va_list		args;

	va_start(args, fmt);
	vprintf(_(fmt), args);
	va_end(args);

	fflush(stdout);
}

void
conduct_check(bool (*check_function) (void))
{
	if (check_function())
	{
		check_ok();
		return;
	}

	pg_log(PG_FATAL, "One or more checks failed. See output above.\n");
}
