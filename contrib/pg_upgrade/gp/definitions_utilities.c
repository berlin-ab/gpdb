#include "pg_upgrade.h"

#include "check.h"

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
