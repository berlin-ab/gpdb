/*
 *	relfilenode_gp.c
 *
 *	functions to manipulate relfilenodes for greenplum
 *
 *	Copyright (c) 2017-Present, Pivotal Software Inc.
 */

#include "postgres_fe.h"
#include "pg_upgrade_greenplum.h"

/*
 * get_pg_database_relfilenode()
 *
 *	Retrieves the relfilenode for a few system-catalog tables.  We need these
 *	relfilenodes later in the upgrade process.
 */
static char *
get_gp_segment_configuration_path(ClusterInfo *cluster)
{
	PGconn	   *conn = connectToServer(cluster, "template1");
	PGresult   *res;
	int			i_relfile;

	res = executeQueryOrDie(conn, "select pg_relation_filepath('gp_segment_configuration') as gp_segment_configuration_path");

	i_relfile = PQfnumber(res, "gp_segment_configuration_path");
	char *gp_segment_configuration_path = pg_strdup(PQgetvalue(res, 0, i_relfile));

	PQclear(res);
	PQfinish(conn);

	return gp_segment_configuration_path;
}


void
truncate_gp_segment_configuration_relfilenode(ClusterInfo *cluster)
{
	char		new_file[MAXPGPATH * 3];
	char *gp_segment_configuration_path;
	FILE *gp_segment_configuration_file;

	gp_segment_configuration_path = get_gp_segment_configuration_path(cluster);
	gp_segment_configuration_file = fopen(gp_segment_configuration_path, "w");

	if (errno != ENOENT && errno != ENOTDIR)
	{
		pg_fatal("could not open file \"%s\" for reading: %s\n",
		         gp_segment_configuration_path, getErrorText());

		return;
	}

	fclose(gp_segment_configuration_file);
}
