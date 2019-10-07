#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdbool.h>

#include "cmockery.h"

#include "pg_upgrade.h"

/* 
 * Test state
 */
ClusterInfo new_cluster;
ClusterInfo old_cluster;
ClusterInfo *test_cluster;

OSInfo      os_info;
OSInfo      *test_os_info;

bool _can_use_file_for_old_tablespace;


/*
 * Dummy functions:
 * 
 * used to provide completely invalid results, not necessary for tests 
 * to run successfully
 */

void
pg_fatal(const char *fmt, ...)
{
	exit(0);
}

void
report_status(eLogType type, const char *fmt, ...)
{

}

PGconn *
connectToServer(ClusterInfo *cluster, const char *db_name)
{
	return NULL;
}

PGresult *
executeQueryOrDie(PGconn *conn, const char *fmt, ...)
{
	return NULL;
}

const char *
getErrorText(void)
{
	return NULL;
}

/*
 * Stub functions:
 */

/*
 * implements can_use_file_for_old_tablespaces to return stubbed value
 */
bool 
can_use_file_for_old_tablespaces(OldTablespaceFileContents *contents)
{
	return _can_use_file_for_old_tablespace;
}

/* 
 * allows test to stub value for can_use_file_for_old_tablespaces
 */
static void 
stub_can_use_file_for_old_tablespaces(bool value)
{
	_can_use_file_for_old_tablespace = value;
}

/*
 * Test hooks
 */
static void
setup(void **state)
{
	test_cluster = malloc(sizeof(ClusterInfo));
	test_os_info = malloc(sizeof(OSInfo));
	old_cluster  = *test_cluster;
	os_info = *test_os_info;

	old_cluster.old_tablespace_file_contents = NULL;
	stub_can_use_file_for_old_tablespaces(false);
}

static void
teardown(void **state)
{
	free(test_cluster);
	free(test_os_info);
}

/*
 * Tests
 */
static void
test_when_postgres_version_8_4_tablespace_directory_suffix_remains_empty(void **state)
{
	old_cluster.major_version = 80400;

	init_tablespaces();

	assert_string_equal(old_cluster.tablespace_suffix, "");
}

static void
test_when_postgres_version_is_before_8_4_tablespace_directory_suffix_remains_empty(
	void **state)
{
	old_cluster.major_version = 80300;

	init_tablespaces();

	assert_string_equal(old_cluster.tablespace_suffix, "");
}

static void
test_when_postgres_version_newer_than_8_4_tablespace_directory_suffix_contains_PG_version_and_catalog_version(
	void **state)
{
	old_cluster.major_version = 80500;
	strcpy(old_cluster.major_version_str, "-SOME_MAJOR_VERSION_STRING-");
	old_cluster.controldata.cat_ver = 12345;

	init_tablespaces();

	assert_string_equal(old_cluster.tablespace_suffix,
	                    "/PG_-SOME_MAJOR_VERSION_STRING-_12345");
}

static void
test_when_postgres_version_matches_gpdb6_postgres_version_tablespace_directory_suffix_contains_GPDB6_tablespace_layout(
	void **state)
{
	old_cluster.major_version = 90400;
	strcpy(old_cluster.major_version_str, "-SOME_MAJOR_VERSION_STRING-");
	old_cluster.controldata.cat_ver = 12345;
	old_cluster.gp_dbid             = 999;

	init_tablespaces();

	assert_string_equal(old_cluster.tablespace_suffix, "/999/GPDB_6_12345");
}

static void
test_it_finds_old_tablespaces_when_provided_as_a_file(void **state)
{
	old_cluster.gp_dbid = 999;
	old_cluster.major_version = 80400;
	new_cluster.major_version = 90400;
	stub_can_use_file_for_old_tablespaces(true);

	OldTablespaceRecord first;
	first.tablespace_path = "/some/directory/for/999";

	OldTablespaceRecord second;
	second.tablespace_path = "/some/other/directory/for/999";

	OldTablespaceRecord *records[] = {&first, &second};

	OldTablespaceFileContents contents;
	contents.number_of_tablespaces = 2;
	contents.old_tablespace_records = &records;

	old_cluster.old_tablespace_file_contents = &contents;

	init_tablespaces();

	assert_int_equal(os_info.num_old_tablespaces, 2);
	assert_string_equal("/some/directory/for/999", os_info.old_tablespaces[0]);
	assert_string_equal("/some/other/directory/for/999", os_info.old_tablespaces[1]);
}

int
main(int argc, char *argv[])
{
	cmockery_parse_arguments(argc, argv);

	const UnitTest tests[] = {
		unit_test_setup_teardown(
			test_it_finds_old_tablespaces_when_provided_as_a_file,
			setup,
			teardown),
		unit_test_setup_teardown(
			test_when_postgres_version_is_before_8_4_tablespace_directory_suffix_remains_empty,
			setup,
			teardown),
		unit_test_setup_teardown(
			test_when_postgres_version_8_4_tablespace_directory_suffix_remains_empty,
			setup,
			teardown),
		unit_test_setup_teardown(
			test_when_postgres_version_newer_than_8_4_tablespace_directory_suffix_contains_PG_version_and_catalog_version,
			setup,
			teardown),
		unit_test_setup_teardown(
			test_when_postgres_version_matches_gpdb6_postgres_version_tablespace_directory_suffix_contains_GPDB6_tablespace_layout,
			setup,
			teardown),
	};

	return run_tests(tests);
}
