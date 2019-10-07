#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cmockery.h"

#include "old_tablespace_file_contents.h"

#include "csv_parser.h"

struct CSV_DocumentData {};

int _faked_number_of_rows = -1;
char *fields[10][10];

char *
CSV_get_field_as_string(CSV_Document *document, int row_index, int field_index)
{
	return fields[row_index][field_index];
}

int
CSV_get_field_as_int(CSV_Document *document, int row_index, int field_index)
{
	return atoi(fields[row_index][field_index]);
}

Oid
CSV_get_field_as_oid(CSV_Document *document, int row_index, int field_index)
{
	return (Oid) strtoul(fields[row_index][field_index], NULL, 10);
}


int
CSV_number_of_rows(CSV_Document *document)
{
	return _faked_number_of_rows;
}

void
CSV_clear_document(CSV_Document *document)
{
}


static void stub_number_of_rows(int new_number_of_rows)
{
	_faked_number_of_rows = new_number_of_rows;
}

static void stub_field(int row_index, int field_index, char *value)
{
	fields[row_index][field_index] = strdup(value);
}

CSV_Document *
CSV_parse_file(FILE *file)
{
	CSV_Document *document = malloc(sizeof(CSV_Document));
	return document;
}

static void setup(void **state)
{
	_faked_number_of_rows = -1;
	
	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 10; j++)
			fields[i][j] = NULL;
}

static void teardown(void **state)
{
	
}

static void
test_it_finds_old_tablespaces_when_provided_as_a_file(void **state)
{
	stub_number_of_rows(2);

	stub_field(0, 0, "123");
	stub_field(0, 1, "456");
	stub_field(0, 2, "/some/directory/for/999");

	stub_field(1, 0, "888");
	stub_field(1, 1, "777");
	stub_field(1, 2, "/some/other/directory/for/999");

	char *path =" /tmp/some/path";
	OldTablespaceFileContents *contents = parse_old_tablespace_file_contents(path);

	assert_int_equal(contents->number_of_tablespaces, 2);

	OldTablespaceRecord *tablespace = contents->old_tablespace_records[0];
	assert_int_equal(123, tablespace->dbid);
	assert_int_equal(456, tablespace->tablespace_oid);
	assert_string_equal("/some/directory/for/999", 
		tablespace->tablespace_path);

	tablespace = contents->old_tablespace_records[1];
	assert_int_equal(888, tablespace->dbid);
	assert_int_equal(777, tablespace->tablespace_oid);
	assert_string_equal("/some/other/directory/for/999", tablespace->tablespace_path);
}

static void
test_it_returns_zero_tablespaces_when_content_is_empty(void **state)
{
	stub_number_of_rows(0);

	char *path = "/tmp/some/path";
	OldTablespaceFileContents *contents = parse_old_tablespace_file_contents(path);

	assert_int_equal(contents->number_of_tablespaces, 0);
}

static void
test_it_can_filter_old_contents_by_dbid(void **state)
{
	OldTablespaceFileContents *contents = make_old_tablespace_file_contents(2);

	contents->old_tablespace_records[0]->dbid = 1;
	contents->old_tablespace_records[0]->tablespace_oid = 123;
	contents->old_tablespace_records[0]->tablespace_path = "";

	contents->old_tablespace_records[1]->dbid = 2;
	contents->old_tablespace_records[1]->tablespace_oid = 456;
	contents->old_tablespace_records[1]->tablespace_path = "";

	OldTablespaceFileContents *filteredContents = filter_old_tablespace_file_for_dbid(contents, 2);

	assert_int_equal(1, filteredContents->number_of_tablespaces);
	assert_int_equal(456, filteredContents->old_tablespace_records[0]->tablespace_oid);
}

static void
test_it_can_return_the_path_of_a_tablespace_for_a_given_oid(void **state)
{
	OldTablespaceFileContents *contents = make_old_tablespace_file_contents(2);

	contents->old_tablespace_records[0]->dbid = 1;
	contents->old_tablespace_records[0]->tablespace_oid = 123;
	contents->old_tablespace_records[0]->tablespace_path = "some path";

	contents->old_tablespace_records[1]->dbid = 1;
	contents->old_tablespace_records[1]->tablespace_oid = 456;
	contents->old_tablespace_records[1]->tablespace_path = "some other path";

	char *found_path = "";

	Oid current_oid = 123;
	found_path = old_tablespace_file_get_tablespace_path_for_oid(contents, current_oid);
	assert_int_not_equal((void *)found_path, NULL);
	assert_string_equal("some path", found_path);

	current_oid = 456;
	found_path = old_tablespace_file_get_tablespace_path_for_oid(contents, current_oid);
	assert_int_not_equal((void *)found_path, NULL);
	assert_string_equal("some other path", found_path);
}


int
main(int argc, char *argv[])
{
	cmockery_parse_arguments(argc, argv);

	const UnitTest tests[] = {
		unit_test_setup_teardown(test_it_finds_old_tablespaces_when_provided_as_a_file, setup, teardown),
		unit_test_setup_teardown(test_it_returns_zero_tablespaces_when_content_is_empty, setup, teardown),
		unit_test_setup_teardown(test_it_can_filter_old_contents_by_dbid, setup, teardown),
		unit_test_setup_teardown(test_it_can_return_the_path_of_a_tablespace_for_a_given_oid, setup, teardown),
	};

	return run_tests(tests);
}
