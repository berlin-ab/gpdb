#include "test_utils.h"

void
executeQuery(PGconn *conn, const char *fmt,...)
{
	static char command[8192];
	va_list		args;

	va_start(args, fmt);
	vsnprintf(command, sizeof(command), fmt, args);
	va_end(args);

	PQexec(conn, command);
}

void
setup_old_cluster()
{
	char *port_string = getenv("PGPORTOLD");

	if (!port_string)
	{
		printf("Must set PGPORTOLD (port number of 5X server) to run tests.\n");
		exit(1);
	}

	old_cluster.port = atoi(port_string);
	old_cluster.major_version = 80300;
}

char *
get_database_name()
{
	return "postgres";
}

void setup_os_info()
{
	os_info.user = "";
}

void
enable_utility_mode()
{
	old_cluster.use_utility_mode = true;
}

void
disable_utility_mode()
{
	old_cluster.use_utility_mode = false;
}
