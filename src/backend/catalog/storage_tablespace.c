#include "catalog/storage_tablespace.h"


static Oid tablespaceMarkedForDeletion;
static void (*unlink_tablespace_dir)(Oid tablespace_for_unlink, bool isRedo);


static void
tablespace_storage_reset(void)
{
	tablespaceMarkedForDeletion = InvalidOid;
}

static void
perform_pending_tablespace_deletion_internal(Oid tablespace_oid_to_delete,
                                                         bool isRedo)
{
	if (tablespace_oid_to_delete == InvalidOid)
		return;

	unlink_tablespace_dir(tablespace_oid_to_delete, isRedo);
	tablespace_storage_reset();
} 

void
TablespaceStorageInit(void (*unlink_tablespace_dir_function)(Oid tablespace_oid, bool isRedo))
{
	unlink_tablespace_dir = unlink_tablespace_dir_function;
	
	tablespace_storage_reset();
}

void
ScheduleTablespaceDirectoryDeletion(Oid tablespaceoid)
{
	tablespaceMarkedForDeletion = tablespaceoid;
}

Oid
GetPendingTablespaceForDeletion()
{
	return tablespaceMarkedForDeletion;
}

void
DoPendingTablespaceDeletion(void)
{
	perform_pending_tablespace_deletion_internal(
		GetPendingTablespaceForDeletion(),
		false
		);
}

void
DoTablespaceDeletion(Oid tablespace_oid_to_delete)
{
	perform_pending_tablespace_deletion_internal(tablespace_oid_to_delete, true);
}
