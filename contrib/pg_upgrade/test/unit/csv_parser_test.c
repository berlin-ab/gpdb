#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdbool.h>

#include "cmockery.h"

#include "csv_parser.h"

static void
test_it_returns_number_of_rows(void **state)
{
	FILE *file = tmpfile();
	fputs("1,joe\n", file);
	rewind(file);

	CSV_Document *document = (CSV_Document*) CSV_parse_file(file);

	fclose(file);

	int number_of_rows = CSV_number_of_rows(document);

	CSV_clear_document(document);

	assert_int_equal(number_of_rows, 1);
}

static void test_it_returns_zero_number_of_rows_for_empty_document(void **state)
{
	FILE *file = tmpfile();

	CSV_Document *document = (CSV_Document*) CSV_parse_file(file);

	fclose(file);

	int number_of_rows = CSV_number_of_rows(document);

	CSV_clear_document(document);

	assert_int_equal(number_of_rows, 0);
}

static void test_it_returns_two_rows_for_document_with_two_lines(void **state)
{
	FILE *file = tmpfile();
	fputs("1,123\n", file);
	fputs("2,456\n", file);
	rewind(file);

	CSV_Document *document = (CSV_Document*) CSV_parse_file(file);

	int number_of_rows = CSV_number_of_rows(document);

	CSV_clear_document(document);

	assert_int_equal(number_of_rows, 2);
}

static void test_the_document_returns_row_data(void **state)
{
	FILE *file = tmpfile();
	fputs("1,123\n", file);
	fputs("2,456\n", file);
	rewind(file);

	CSV_Document *document = (CSV_Document*) CSV_parse_file(file);

	assert_string_equal(CSV_get_field_as_string(document, 0, 0), "1");
	assert_string_equal(CSV_get_field_as_string(document, 0, 1), "123");
	assert_string_equal(CSV_get_field_as_string(document, 1, 0), "2");
	assert_string_equal(CSV_get_field_as_string(document, 1, 1), "456");

	CSV_clear_document(document);
}

static void test_the_document_returns_row_data_as_integers(void **state)
{
	FILE *file = tmpfile();
	fputs("1,123\n", file);
	fputs("2,456\n", file);
	rewind(file);

	CSV_Document *document = (CSV_Document*) CSV_parse_file(file);

	assert_int_equal(CSV_get_field_as_int(document, 0, 0), 1);
	assert_int_equal(CSV_get_field_as_int(document, 0, 1), 123);
	assert_int_equal(CSV_get_field_as_int(document, 1, 0), 2);
	assert_int_equal(CSV_get_field_as_int(document, 1, 1), 456);

	CSV_clear_document(document);
}

int
main(int argc, char *argv[])
{
	cmockery_parse_arguments(argc, argv);

	const UnitTest tests[] = {
		unit_test(test_it_returns_number_of_rows),
		unit_test(test_it_returns_zero_number_of_rows_for_empty_document),
		unit_test(test_it_returns_two_rows_for_document_with_two_lines),
		unit_test(test_the_document_returns_row_data),
		unit_test(test_the_document_returns_row_data_as_integers),
	};

	return run_tests(tests);
}
