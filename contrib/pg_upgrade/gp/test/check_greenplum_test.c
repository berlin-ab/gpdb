#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>

#include "cmockery.h"
#include "../check_greenplum.h"


void check_ok(void) 
{
	
}


void pg_log()
{
	
}


void report_status()
{

}


int number_of_passing_checks;


static bool passing_check(void) {
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


int main(int argc, char *argv[])
{
	cmockery_parse_arguments(argc, argv);

	const UnitTest tests[] = {
		unit_test(test_check_greenplum_runs_all_given_checks)
	};

	return run_tests(tests);
}