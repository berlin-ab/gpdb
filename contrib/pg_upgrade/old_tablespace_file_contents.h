#ifndef OLD_TABLESPACE_FILE_CONTENTS_H
#define OLD_TABLESPACE_FILE_CONTENTS_H

#include "postgres_ext.h"
#include "postgres_fe.h"

typedef struct OldTablespaceRecord {
	char *tablespace_path;
	Oid tablespace_oid;
	int dbid;
} OldTablespaceRecord;

typedef struct OldTablespaceFileContents {
	int number_of_tablespaces;
	OldTablespaceRecord **old_tablespace_records;
} OldTablespaceFileContents;

/*
 * Create an instance of an OldTablespaceFileContents for the number of
 * tablespaces given
 */
OldTablespaceFileContents *
make_old_tablespace_file_contents(int number_of_tablespaces);

/*
 * Return an OldTablespaceFileContents containing tablespaces in the given csv
 * file
 */
OldTablespaceFileContents *
parse_old_tablespace_file_contents(char *file_path);

/*
 * Return an OldTablespaceFileContents containing only tablespaces for the 
 * given dbid
 */
OldTablespaceFileContents *
filter_old_tablespace_file_for_dbid(OldTablespaceFileContents *contents, int dbid);

/*
 * free memory allocated for OldTablespaceFileContents
 */
void clear_old_tablespace_file_contents(OldTablespaceFileContents *contents);

bool can_use_file_for_old_tablespaces(OldTablespaceFileContents *contents);

/*
 * Get the file path for a given old tablespace for the given tablespace oid
 */
char *old_tablespace_file_get_tablespace_path_for_oid(
	OldTablespaceFileContents *contents, Oid oid);

#endif /* OLD_TABLESPACE_FILE_CONTENTS_H */
