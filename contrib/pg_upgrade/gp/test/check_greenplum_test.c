#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>

#include "cmockery.h"
#include "../check_greenplum.h"


static int number_of_passing_checks;
static int number_of_check_ok_checks;
static int number_of_failing_checks;


void check_ok(void) 
{
	number_of_check_ok_checks++;
}


void check_failed(void)
{

}

void pg_log()
{

}


void report_status()
{

}

static bool failing_check(void)
{
	number_of_failing_checks++;
	return false;
}


static bool passing_check(void)
{
	number_of_passing_checks++;
	return true;
}


static void
test_check_greenplum_runs_all_given_checks(void **state)
{
	number_of_passing_checks = 0;

	check_function my_list[] = {
		passing_check,
		passing_check
	};

	check_greenplum(my_list, 2);

	assert_int_equal(number_of_passing_checks, 2);
}


static void
test_check_greenplum_calls_check_ok_for_success(void **state)
{
	number_of_check_ok_checks = 0;

	check_function my_list[] = {
		passing_check,
		passing_check
	};

	check_greenplum(my_list, 2);

	assert_int_equal(number_of_check_ok_checks, 2);
}


static void
test_check_greenplum_calls_gp_report_failure_on_failure(void **state)
{
	number_of_failing_checks = 0;

	check_function my_list[] = {
		passing_check,
		failing_check
	};

	check_greenplum(my_list, 2);

	assert_int_equal(number_of_failing_checks, 1);
}


int main(int argc, char *argv[])
{
	cmockery_parse_arguments(argc, argv);

	const UnitTest tests[] = {
		unit_test(test_check_greenplum_runs_all_given_checks),
		unit_test(test_check_greenplum_calls_check_ok_for_success),
		unit_test(test_check_greenplum_calls_gp_report_failure_on_failure)
	};

	return run_tests(tests);
}