#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>

#include "cmockery.h"
#include "stdbool.h"


#include "../checks.h"
#include "../../pg_upgrade.h"
#include "libpq-fe.h"

ClusterInfo old_cluster;
ClusterInfo new_cluster;
OSInfo os_info;
UserOpts user_opts;


bool
exec_prog(const char *log_file, const char *opt_log_file,
          bool throw_error, const char *fmt,...){
	return true;
}


/*
 * executeQueryOrDie()
 *
 *	Formats a query string from the given arguments and executes the
 *	resulting query.  If the query fails, this function logs an error
 *	message and calls exit() to kill the program.
 */
static void
executeQuery(PGconn *conn, const char *fmt,...)
{
	static char command[8192];
	va_list		args;

	va_start(args, fmt);
	vsnprintf(command, sizeof(command), fmt, args);
	va_end(args);

	PQexec(conn, command);
}

static void 
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


static char * 
get_database_name()
{
	return "postgres";
}


void setup_os_info()
{
	os_info.user = "";
}

static void 
enable_utility_mode()
{
	old_cluster.use_utility_mode = true;
}


static void
disable_utility_mode()
{
	old_cluster.use_utility_mode = false;
}


static void 
test_partition_table_with_index_after_exchange_should_fail(void **state)
{
	PGconn	   *connection;

	get_db_and_rel_infos(&old_cluster);

	disable_utility_mode();

	connection = connectToServer(&old_cluster, get_database_name());
	executeQuery(connection, "create schema greenplum_pg_upgrade_integration_test;");
	executeQuery(connection, "set search_path to greenplum_pg_upgrade_integration_test; ");
	executeQuery(connection, "create table t (a integer, b text, c integer) with (appendonly=true) distributed by (a) partition by range(c) (start(1) end(3) every(1)); ");
	executeQuery(connection, "create index t_idx on t (b);");
	executeQuery(connection, "create table t_exch (a integer, b text, c integer) with (appendonly=true) distributed by (a);");
	executeQuery(connection, "alter table t exchange partition for (rank(1)) with table t_exch;");

	enable_utility_mode();

	bool result = check_covering_aoindex();

	assert_false(result);

	executeQueryOrDie(connection, "drop schema greenplum_pg_upgrade_integration_test CASCADE;");
	PQfinish(connection);
}


int main(int argc, char *argv[])
{
	cmockery_parse_arguments(argc, argv);

	setup_old_cluster();
	setup_os_info();

	const		UnitTest tests[] = {
		unit_test(test_partition_table_with_index_after_exchange_should_fail)
	};

	return run_tests(tests);
}