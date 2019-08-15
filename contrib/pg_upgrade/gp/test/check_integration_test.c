#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>

#include "cmockery.h"
#include "stdbool.h"


#include "../checks.h"
#include "../../pg_upgrade.h"

ClusterInfo old_cluster;
ClusterInfo new_cluster;
OSInfo os_info;
UserOpts user_opts;


bool
exec_prog(const char *log_file, const char *opt_log_file,
          bool throw_error, const char *fmt,...){
	return true;
}


void 
test_check_fails(void **state)
{
	PGconn	   *conn;
	PGresult   *res;

	os_info.user = "adamberlin";
	old_cluster.port = 5000;
	old_cluster.major_version = 80300;

	get_db_and_rel_infos(&old_cluster);

	conn = connectToServer(&old_cluster, os_info.user);
	res = executeQueryOrDie(conn, "select ();");


	bool result = check_covering_aoindex();

	assert_false(result);
}


int main(int argc, char *argv[])
{
	cmockery_parse_arguments(argc, argv);
	
	const		UnitTest tests[] = {
		unit_test(test_check_fails)
	};

	return run_tests(tests);
}