/*-------------------------------------------------------------------------
 *
 * info_gp.h
 *
 * Greenplum specific logic for determining tablespace paths
 * for a given tablespace_oid.
 *
 * Copyright (c) 2019-Present Pivotal Software, Inc.
 */


#ifndef PG_UPGRADE_INFO_GP_H
#define PG_UPGRADE_INFO_GP_H

#include "greenplum/old_tablespace_file_contents.h"

typedef enum GetTablespacePathResponseCodes {
	GetTablespacePathResponse_MISSING_FILE,
	GetTablespacePathResponse_FOUND,
	GetTablespacePathResponse_NOT_FOUND_IN_FILE,
} GetTablespacePathResponseCodes;

typedef struct GetTablespacePathResponse {
	GetTablespacePathResponseCodes code;
	char *tablespace_path;
} GetTablespacePathResponse;

/*
 * Return the Tablespace OID specific tablespace path to an GDPB 5 tablespace
 */
GetTablespacePathResponse
gp_get_tablespace_path(OldTablespaceFileContents *contents, Oid tablespace_oid);

#endif /* PG_UPGRADE_INFO_GP_H */