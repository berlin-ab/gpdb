#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


#include "old_tablespace_file_contents.h"
#include "csv_parser.h"

static int
number_of_tablespaces_matching_dbid(OldTablespaceFileContents *originalContents, int dbid_to_find)
{
	int number_of_tablespaces = 0;

	for(int i = 0; i < originalContents->number_of_tablespaces; i++)
		if (originalContents->old_tablespace_records[i]->dbid == dbid_to_find)
			number_of_tablespaces++;

	return number_of_tablespaces;
}

static bool
matches_dbid(OldTablespaceRecord *record, int dbid)
{
	return record->dbid == dbid;
}

static void
populate_tablespace_record(OldTablespaceRecord *originalRecord, OldTablespaceRecord *newRecord)
{
	newRecord->dbid            = originalRecord->dbid;
	newRecord->tablespace_oid  = originalRecord->tablespace_oid;
	newRecord->tablespace_path = originalRecord->tablespace_path;
}

static void
populate_record_from_csv(OldTablespaceRecord *record, CSV_Document *document, int row_index)
{
	record->dbid = CSV_get_field_as_int(document, row_index, 0);
	record->tablespace_oid = CSV_get_field_as_oid(document, row_index, 1);
	record->tablespace_path = CSV_get_field_as_string(document, row_index, 2);
}

static void
populate_old_tablespace_records(OldTablespaceFileContents *contents, int number_of_tablespaces)
{
	contents->old_tablespace_records = palloc0(sizeof(OldTablespaceRecord *) * number_of_tablespaces);

	for (int i = 0; i < number_of_tablespaces; i++)
		contents->old_tablespace_records[i] = palloc0(sizeof(OldTablespaceRecord));
}

static CSV_Document *
get_csv_document(char *file_path)
{
	FILE *file = fopen(file_path, "r");

	CSV_Document *document = CSV_parse_file(file);

	fclose(file);

	return document;
}

bool
can_use_file_for_old_tablespaces(OldTablespaceFileContents *contents)
{
	return contents && contents->number_of_tablespaces > 0;
}

OldTablespaceFileContents *
make_old_tablespace_file_contents(int number_of_tablespaces)
{
	OldTablespaceFileContents *contents = palloc0(sizeof(OldTablespaceFileContents));
	contents->number_of_tablespaces = number_of_tablespaces;
	populate_old_tablespace_records(contents, number_of_tablespaces);

	return contents;
}

void
clear_old_tablespace_file_contents(OldTablespaceFileContents *contents)
{
	for (int i = 0; i < contents->number_of_tablespaces; i++)
		pfree(contents->old_tablespace_records[i]);

	pfree(contents);
}

/*
 * expects csv file to have the fields:
 * 
 * "dbid","tablespace_oid","path"
 * 
 */
OldTablespaceFileContents *
parse_old_tablespace_file_contents(char *file_path)
{
	CSV_Document *document = get_csv_document(file_path);

	OldTablespaceFileContents *contents = make_old_tablespace_file_contents(
		CSV_number_of_rows(document));

	for (int i = 0; i < contents->number_of_tablespaces; i++)
		populate_record_from_csv(contents->old_tablespace_records[i], document, i);

	CSV_clear_document(document);

	return contents;
}

OldTablespaceFileContents*
filter_old_tablespace_file_for_dbid(OldTablespaceFileContents *originalContents, int dbid_to_find)
{
	int match_index = 0;

	OldTablespaceFileContents *result = make_old_tablespace_file_contents(
		number_of_tablespaces_matching_dbid(originalContents, dbid_to_find));

	for(int i = 0; i < originalContents->number_of_tablespaces; i++)
	{
		OldTablespaceRecord *originalRecord = originalContents->old_tablespace_records[i];
		OldTablespaceRecord *newRecord = result->old_tablespace_records[match_index];

		if (matches_dbid(originalRecord, dbid_to_find))
		{
			match_index++;
			populate_tablespace_record(originalRecord, newRecord);
		}
	}

	return result;
}

char *
old_tablespace_file_get_tablespace_path_for_oid(OldTablespaceFileContents *contents, Oid tablespace_oid)
{
	OldTablespaceRecord *currentRecord;

	for (int i = 0; i < contents->number_of_tablespaces; i++)
	{
		currentRecord = contents->old_tablespace_records[i];

		if (currentRecord->tablespace_oid == tablespace_oid)
			return currentRecord->tablespace_path;
	}

	return NULL;
}
