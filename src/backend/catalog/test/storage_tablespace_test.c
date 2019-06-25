#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include "cmockery.h"

#include "postgres.h"
#include "utils/memutils.h"
#include "utils/palloc.h"

#include "catalog/storage_tablespace.h"
#include <stdio.h>


static Oid unlink_called_with_tablespace_oid;
static bool unlink_called_with_redo;
static Oid NOT_CALLED_OID = -1000;
static bool assertion_thrown = false;

/*
 * Redefine Assert infrastructure
 */
bool assert_enabled = true;

void ExceptionalCondition(const char *conditionName,
							const char *errorType,
							const char *fileName, int lineNumber) __attribute__((noreturn)) {
	assertion_thrown = true;
}

/*
 * Test helpers
 */
static void reset_exceptions() {
	assertion_thrown = false;
}


static void unlink_tablespace_directory(Oid tablespaceOid, bool isRedo) {
	unlink_called_with_tablespace_oid = tablespaceOid;
	unlink_called_with_redo = isRedo;
}


void
setup()
{
	unlink_called_with_redo = false;
	unlink_called_with_tablespace_oid = NOT_CALLED_OID;

	reset_exceptions();
	TablespaceStorageInit(unlink_tablespace_directory);
}

/*
 * Tests
 */
void
test__create_tablespace_storage_populates_the_pending_tablespace_deletes_list(
	void **state)
{
	setup();

	Oid someTablespaceOid = 17999;

	ScheduleTablespaceDirectoryDeletion(someTablespaceOid);

	Oid tablespaceForDeletion = GetPendingTablespaceForDeletion();

	assert_int_equal(17999, tablespaceForDeletion);

	setup();

	someTablespaceOid = 88888;

	ScheduleTablespaceDirectoryDeletion(someTablespaceOid);

	tablespaceForDeletion = GetPendingTablespaceForDeletion();

	assert_int_equal(88888, tablespaceForDeletion);
}

void
test__get_pending_tablespace_for_deletion_returns_a_null_value_by_default(void **state)
{
	setup();

	Oid tablespaceForDeletion = GetPendingTablespaceForDeletion();

	assert_int_equal(InvalidOid, tablespaceForDeletion);
}

void
test__DoPendingTablespaceDeletion_removes_the_pending_tablespace_for_deletion_so_it_is_not_deleted_by_the_next_transaction(
	void **state)
{
	setup();

	Oid someTablespaceOid = 99999;

	ScheduleTablespaceDirectoryDeletion(someTablespaceOid);

	DoPendingTablespaceDeletion();

	Oid tablespaceForDeletion = GetPendingTablespaceForDeletion();

	assert_int_equal(InvalidOid, tablespaceForDeletion);
}

void
test__DoPendingTablespaceDeletion_calls_unlink(void **state)
{
	setup();

	ScheduleTablespaceDirectoryDeletion(99999);

	DoPendingTablespaceDeletion();

	assert_int_equal(unlink_called_with_tablespace_oid, 99999);
	assert_int_equal(unlink_called_with_redo, false);
}

void
test__delete_called_when_invalid_tablespace_set_does_not_call_unlink(void **state) 
{
	setup();

	ScheduleTablespaceDirectoryDeletion(InvalidOid);

	DoPendingTablespaceDeletion();

	assert_int_equal(unlink_called_with_tablespace_oid, NOT_CALLED_OID);
}

void
test__DoTablespaceDeletion_calls_unlink_with_tablespace_oid_and_redo_flag(void **state) {
	setup();

	ScheduleTablespaceDirectoryDeletion(66666);

	DoTablespaceDeletion(77777);

	assert_int_equal(unlink_called_with_tablespace_oid, 77777);
	assert_int_equal(unlink_called_with_redo, true);
}

void test__UnscheduleTablespaceDirectoryDeletion_removes_the_scheduled_tablespace_for_deletion(void **state)
{
	setup();

	ScheduleTablespaceDirectoryDeletion(66666);
	UnscheduleTablespaceDirectoryDeletion();

	Oid tablespaceForDeletion = GetPendingTablespaceForDeletion();

	assert_int_equal(tablespaceForDeletion, InvalidOid);

}

void test__an_UnscheduleTablespaceDirectoryDeletion_does_not_get_unlinked(void **state)
{
	setup();

	ScheduleTablespaceDirectoryDeletion(66666);
	UnscheduleTablespaceDirectoryDeletion();
	
	DoPendingTablespaceDeletion();

	assert_int_equal(unlink_called_with_tablespace_oid, NOT_CALLED_OID);
}

void test__initializing_multiple_times_will_throw_an_assert_error(void **state) 
{
	setup();

	TablespaceStorageInit(unlink_tablespace_directory);

	reset_exceptions();

	assert_false(assertion_thrown);

	TablespaceStorageInit(unlink_tablespace_directory);

	assert_true(assertion_thrown);
}

int
main(int argc, char *argv[])
{
	cmockery_parse_arguments(argc, argv);

	const UnitTest tests[] = {
		unit_test(
			test__create_tablespace_storage_populates_the_pending_tablespace_deletes_list),
		unit_test(
			test__get_pending_tablespace_for_deletion_returns_a_null_value_by_default),
		unit_test(
			test__DoPendingTablespaceDeletion_removes_the_pending_tablespace_for_deletion_so_it_is_not_deleted_by_the_next_transaction
		),
		unit_test(
			test__DoPendingTablespaceDeletion_calls_unlink
		),
		unit_test(
			test__delete_called_when_invalid_tablespace_set_does_not_call_unlink
			),
		unit_test(
			test__DoTablespaceDeletion_calls_unlink_with_tablespace_oid_and_redo_flag
			),
		unit_test(
			test__UnscheduleTablespaceDirectoryDeletion_removes_the_scheduled_tablespace_for_deletion
			),
		unit_test(
			test__an_UnscheduleTablespaceDirectoryDeletion_does_not_get_unlinked
			),
		unit_test(
			test__initializing_multiple_times_will_throw_an_assert_error
			)
	};

	return run_tests(tests);
}
