#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <string.h>

#include "cmockery.h"
#include "libpq-fe.h"
#include "stdbool.h"
#include "stdlib.h"
#include "sys/stat.h"

#include "utilities/gpdb5-cluster.h"
#include "utilities/gpdb6-cluster.h"
#include "utilities/upgrade-helpers.h"
#include "utilities/query-helpers.h"
#include "utilities/test-helpers.h"

#include "bdd-library/bdd.h"

#include "tablespace.h"

#define EXPECTED_TABLESPACE_MAP_FILE_PATH "utilities/expected_tablespace_map"
#define ACTUAL_TABLESPACE_MAP_FILE_PATH "tablespace_map"

static char tablespace_oid5[10];


static void
createFilespaceWithTablespaceInFiveCluster(void)
{
	PGconn *connection5 = connectToFive();
	PGresult *result5;

	/*
	 * Create filespace directories
	 */
	system("rm -rf /tmp/fsseg{-1,0,1,2}");
	system("rm -rf /tmp/fsdummy{1,2,3,4}");

	/*
	 * Create filespace and tablespace within the filespace.
	 * Note that supplying identical location dirs for the primary and mirror
	 * in a primary-mirror pair requires a multi-node test setup. 
	 * These locations are ignored in the filespace->tablespace upgrade as the
	 * primary locations are used to generate the necessary DDL and tablespace
	 * map. Thus, we supply dummy directories here just to make the syntax
	 * check happy.
	 */
	result5 = executeQuery(connection5, "CREATE FILESPACE some_filespace ( \n"
						"1: '/tmp/fsseg-1/', \n"
						"2: '/tmp/fsseg0/', \n"
						"3: '/tmp/fsseg1/', \n"
						"4: '/tmp/fsseg2/', \n"
						"5: '/tmp/fsdummy1/', \n"
						"6: '/tmp/fsdummy2/', \n"
						"7: '/tmp/fsdummy3/', \n"
						"8: '/tmp/fsdummy4/' );");
	PQclear(result5);

	result5 = executeQuery(connection5, "CREATE TABLESPACE some_tablespace FILESPACE some_filespace;");
	result5 = executeQuery(connection5, "SELECT oid FROM pg_tablespace WHERE spcname='some_tablespace';");
	strcpy(tablespace_oid5, PQgetvalue(result5, 0, 0));
	result5 = executeQuery(connection5, "VACUUM FREEZE;");
	PQclear(result5);

	PQfinish(connection5);
}

static void
tablespaceShouldHaveBeenCreatedOnSixClusterMaster(void)
{
	PGconn *connection6 = connectToSix();
	PGresult *result6;
	char query[500];
	char *spcoid6;
	char *spclocation6;
	char expected_master_spclocation[100];

	/*
	 * Assert that the tablespace exists and has the same oid as the old
	 * cluster.
	 */
	printf("tablespace_oid5 = %s\n", tablespace_oid5);
	result6 = executeQuery(connection6, "SELECT oid FROM pg_tablespace "
										"WHERE spcname = 'ts'");
	assert_int_equal(PQntuples(result6), 1);
	spcoid6 = PQgetvalue(result6, 0, 0);
	assert_string_equal(spcoid6, tablespace_oid5);
	PQclear(result6);

	/*
	 * Assert expected master tablespace location.
	 */
	sprintf(query, "SELECT pg_tablespace_location(%s)", spcoid6);
	result6 = executeQuery(connection6, query);
	spclocation6 = PQgetvalue(result6, 0, 0);
	sprintf(expected_master_spclocation, "/tmp/fsseg-1/GP6/%s", spcoid6);
	assert_string_equal(spclocation6, expected_master_spclocation);
	PQclear(result6);

	PQfinish(connection6);
}

static void
anAdministratorPerformsAnUpgrade()
{
	performUpgrade();
}

void
test_a_filespace_with_tablespace_can_be_upgraded(void **state)
{
	given(createFilespaceWithTablespaceInFiveCluster);
	when(anAdministratorPerformsAnUpgrade);
	then(tablespaceShouldHaveBeenCreatedOnSixClusterMaster);
}
