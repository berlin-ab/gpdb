#include "checks.h"


/*
 *	check_covering_aoindex
 *
 *	A partitioned AO table which had an index created on the parent relation,
 *	and an AO partition exchanged into the hierarchy without any indexes will
 *	break upgrades due to the way pg_dump generates DDL.
 *
 *	create table t (a integer, b text, c integer)
 *		with (appendonly=true)
 *		distributed by (a)
 *		partition by range(c) (start(1) end(3) every(1));
 *	create index t_idx on t (b);
 *
 *	At this point, the t_idx index has created AO blockdir relations for all
 *	partitions. We now exchange a new table into the hierarchy which has no
 *	index defined:
 *
 *	create table t_exch (a integer, b text, c integer)
 *		with (appendonly=true)
 *		distributed by (a);
 *	alter table t exchange partition for (rank(1)) with table t_exch;
 *
 *	The partition which was swapped into the hierarchy with EXCHANGE does not
 *	have any indexes and thus no AO blockdir relation. This is in itself not
 *	a problem, but when pg_dump generates DDL for the above situation it will
 *	create the index in such a way that it covers the entire hierarchy, as in
 *	its original state. The below snippet illustrates the dumped DDL:
 *
 *	create table t ( ... )
 *		...
 *		partition by (... );
 *	create index t_idx on t ( ... );
 *
 *	This creates a problem for the Oid synchronization in pg_upgrade since it
 *	expects to find a preassigned Oid for the AO blockdir relations for each
 *	partition. A longer term solution would be to generate DDL in pg_dump which
 *	creates the current state, but for the time being we disallow upgrades on
 *	cluster which exhibits this.
 */
bool
check_covering_aoindex(void)
{
	char		output_path[MAXPGPATH];
	FILE	   *script = NULL;
	bool		found = false;
	int			dbnum;

	prep_status("Checking for non-covering indexes on partitioned AO tables");

	snprintf(output_path, sizeof(output_path), "mismatched_aopartition_indexes.txt");

	for (dbnum = 0; dbnum < old_cluster.dbarr.ndbs; dbnum++)
	{
		PGresult   *res;
		PGconn	   *conn;
		int			ntups;
		int			rowno;
		DbInfo	   *active_db = &old_cluster.dbarr.dbs[dbnum];

		conn = connectToServer(&old_cluster, active_db->db_name);
		res = executeQueryOrDie(conn,
		                        "SELECT DISTINCT ao.relid, inh.inhrelid "
		                        "FROM   pg_catalog.pg_appendonly ao "
		                        "       JOIN pg_catalog.pg_inherits inh "
		                        "         ON (inh.inhparent = ao.relid) "
		                        "       JOIN pg_catalog.pg_appendonly aop "
		                        "         ON (inh.inhrelid = aop.relid AND aop.blkdirrelid = 0) "
		                        "       JOIN pg_catalog.pg_index i "
		                        "         ON (i.indrelid = ao.relid) "
		                        "WHERE  ao.blkdirrelid <> 0;");

		ntups = PQntuples(res);

		if (ntups > 0)
		{
			found = true;

			if (script == NULL && (script = fopen(output_path, "w")) == NULL)
				pg_log(PG_FATAL, "Could not create necessary file:  %s\n",
				       output_path);

			for (rowno = 0; rowno < ntups; rowno++)
			{
				fprintf(script, "Mismatched index on partition %s in relation %s\n",
				        PQgetvalue(res, rowno, PQfnumber(res, "inhrelid")),
				        PQgetvalue(res, rowno, PQfnumber(res, "relid")));
			}
		}

		PQclear(res);
		PQfinish(conn);
	}

	if (found)
	{
		fclose(script);
		gp_check_failure(
			"| Your installation contains partitioned append-only tables\n"
			"| with an index defined on the partition parent which isn't\n"
			"| present on all partition members.  These indexes must be\n"
			"| dropped before the upgrade.  A list of relations, and the\n"
			"| partitions in question is in the file:\n"
			"| \t%s\n\n", output_path);
		return false;

	}

	return true;
}
